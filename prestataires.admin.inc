<?php

function prestataires_settings_form($form, &$form_state) {

    $settings = variable_get('prestataires', array(
        'default_owner' => 1,
    ));

    $form = array(
        '#tree' => true,

        'default_owner' => array(
            '#type' => 'textfield',
            '#title' => t("Nodes owner UID by default"),
            '#default_value' => $settings['default_owner'],
            '#size' => 3,
            '#required' => true,
        ),

        'save' => array(
            '#type' => 'submit',
            '#value' => t("Save"),
        ),
    );

    return $form;
}

function prestataires_settings_form_submit($form, &$form_state) {

    $settings = array(
        'default_owner' => $form_state['values']['default_owner'],
    );

    variable_set('prestataires', $settings);
}

function prestataires_user_admin_form($form, &$form_state, $user) {

    $settings = variable_get('prestataires', array());

    $form = array(
        '#tree' => true,

        'nodes_container' => array(
            '#type' => 'container',
            '#theme' => 'prestataires_user_admin_form_nodes',

            '##functionalities_labels' => array(),

            'nodes' => array(),

            'add_node' => array(

                'node' => array(
                    '#type' => 'textfield',
                    '#title' => t("Associate a node"),
                    '#description' => t("(*) : currently owned by '@username'", array('@username' => user_load($settings['default_owner'])->name)),
                    '#autocomplete_path' => 'admin/prestataires/node_autocomplete',
                ),

                'add' => array(
                    '#type' => 'submit',
                    '#value' => t("Add"),
                    '#submit' => array('prestataires_user_admin_form_add_submit'),
                    '#validate' => array('prestataires_user_admin_form_add_validate'),
                ),
            ),
        ),

        'save' => array(
            '#type' => 'submit',
            '#value' => t("Save"),
        ),
    );

    $prestataires_modules = module_implements('prestataires_infos');

    $nodes_functionalities = array();

    foreach($prestataires_modules as $module) {
        $functionalities = module_invoke($module, 'prestataires_infos', null);

        foreach($functionalities as $functionality => $infos) {

            switch($infos['group']) {

                case 'nodes':
                    $nodes_functionalities[$functionality . '@' . $module] = $infos;
                    $form['nodes_container']['##functionalities_labels'][] = $infos['label'];
                    break;
            }
        }
    }

    $results = db_query(
        "SELECT p.*, n.title, n.title IS NULL as node_deleted FROM {prestataires} AS p
        LEFT JOIN {node} AS n ON n.nid=p.nid
        WHERE p.uid=?
        ORDER BY n.title IS NOT NULL DESC, n.title ASC",
        array(
            $user->uid,
        )
    );

    while($row = $results->fetchObject()) {

        $params = unserialize($row->params);

        if($row->node_deleted == false) {
            $title = l($row->title, drupal_get_path_alias('node/' . $row->nid), array('attributes' => array('target' => "_blank")));
        } else {
            $title = $params['title'] . " (" . t("deleted node") . ")";
        }

        $form['nodes_container']['nodes'][$row->nid] = array(
            'title' => array(
                '#markup' => $title,
            ),

            'functionalities' => array(),

            'delete' => $row->node_deleted == false ?
                array(
                    '#type' => 'submit',
                    '#value' => t("Delete"),
                    '#submit' => array('prestataires_user_admin_form_delete_submit'),
                    '#name' => 'delete_' . $row->nid,
                ) :
                array(
                    '#markup' => t("A tool is currently in development"),
                ),
        );

        foreach($nodes_functionalities as $node_functionality => $infos) {

            if($infos['filter_callback']($row->nid, $row->uid) != true) {

                $form['nodes_container']['nodes'][$row->nid]['functionalities'][$node_functionality] = array();
                continue;
            }

            $form['nodes_container']['nodes'][$row->nid]['functionalities'][$node_functionality] = array(
                '#type' => 'checkbox',
                '#title' => null,
                '#default_value' => $params['functionalities'][$node_functionality],
            );
        }
    }

    return $form;
}

function prestataires_user_admin_form_autocomplete($string) {

    $settings = variable_get('prestataires', array());

    $output = array();

    if(strlen($string) > 2) {
        $results = db_query(
            "SELECT n.nid, n.title, p.uid=? AS owned FROM {node} AS n
            LEFT JOIN {prestataires} AS p ON p.nid=n.nid
            WHERE n.title LIKE ?
            AND (p.nid IS NULL OR p.uid=?)
            AND (n.language=? OR n.language='und')
            ORDER BY n.title ASC",
            array(
                $settings['default_owner'],
                "%" . $string . "%",
                $settings['default_owner'],
                language_default('language'),
            )
        );

        while($row = $results->fetchObject()) {
            $output[$row->title . ' [nid:' . $row->nid . ']'] = ($row->owned == true ? " (*)" : "") . $row->title;
        }
    }

    drupal_json_output($output);
}

function prestataires_user_admin_form_add_validate($form, &$form_state) {
    // DEV : todo
}

function prestataires_user_admin_form_add_submit($form, &$form_state) {

    $settings = variable_get('prestataires', array());

    $parsed = array();
    preg_match("#\[nid\:([0-9]+)\]#", $form_state['values']['nodes_container']['add_node']['node'], $parsed);

    $uid = $form_state['build_info']['args'][0]->uid;
    $nid = $parsed[1];

    $default_owned = db_query(
        "SELECT * FROM {prestataires} WHERE uid=? AND nid=?",
        array(
            $settings['default_owner'],
            $nid,
        )
    )->fetchObject();

    // le node n'est pas attribué (pas-même au propriétaire par défaut), et ne possède donc aucune donnée
    if(empty($default_owned)) {

        db_query(
            "INSERT INTO {prestataires} SET uid=?, nid=?, params=?", array(
            $uid,
            $nid,
            serialize(array(
                'title' => "",              // pour du debuggage + repérage des data orphelines
                'previous_uid' => "",       // ---
                'functionalities' => array(),                
            )),
        ));

        // on informe les modules de la nouvelle association
        module_invoke_all('prestataires_node_add', $nid, $uid);
        
        // on propose aux modules d'associer au node des données qui leur sont propres, dans le contexte du module Prestataires
        $modules = module_implements('prestataires_node_data');

        foreach($modules as $module) {
            $data = module_invoke($module, 'prestataires_node_data', $nid, $uid);

            $record = array(
                'nid' => $nid,
                'module' => $module,
                'data' => $data,
            );

            drupal_write_record('prestataires_nodes_data', $record);
        }

    } else {

        db_query(
            "DELETE FROM {prestataires} WHERE uid=? AND nid=?",
            array(
                $settings['default_owner'],
                $nid,
            )
        );

        $default_owned->uid = $uid; // réattribution

        drupal_write_record('prestataires', $default_owned);
    }
}

function prestataires_user_admin_form_delete_submit($form, &$form_state) {

    $settings = variable_get('prestataires', array());

    $parents = array_values($form_state['clicked_button']['#parents']);
    array_pop($parents);
    $nid = array_pop($parents);
    $uid = $form_state['build_info']['args'][0]->uid;

    // le user en cours d'édition n'est pas celui qui a la propriété des nodes par défaut
    if($uid != $settings['default_owner']) {
        db_query(
            "UPDATE {prestataires} SET uid=? WHERE uid=? AND nid=?",
            array(
                $settings['default_owner'],
                $uid,
                $nid,
            )
        );

        drupal_set_message(t("Node has been transfered on '@owner' account", array('@owner' => user_load($settings['default_owner'])->name)), 'status');

    } else {
        // DEV : hook de nettoyage des données

        db_query(
            "DELETE FROM {prestataires} WHERE uid=? AND nid=?",
            array(
                $uid,
                $nid,
            )
        );

        drupal_set_message(t("Node has been dissociated. Its whole data have been deleted."), 'status');
    }
}

function prestataires_user_admin_form_submit($form, &$form_state) {

    if(empty($form_state['values']['nodes_container']['nodes'])) {return;}

    $uid = $form_state['build_info']['args'][0]->uid;

    foreach($form_state['values']['nodes_container']['nodes'] as $nid => $values) {

        $record = db_query("SELECT * FROM {prestataires} WHERE uid=? AND nid=?", array(
            $uid,
            $nid,
        ))->fetchObject();

        $params = unserialize($record->params);
        $params['functionalities'] = $values['functionalities'];
        $record->params = serialize($params);

        drupal_write_record('prestataires', $record, array('uid', 'nid'));
    }

    drupal_set_message(t("Parameters have been saved"));
}

function theme_prestataires_user_admin_form_nodes($vars) {
    $output = "";

    $element = $vars['element'];

    $rows = array();

    $nodes = element_children($element['nodes']);

    foreach($nodes as $nid) {
        $rows[$nid] = array(render($element['nodes'][$nid]['title']));

        foreach(element_children($element['nodes'][$nid]['functionalities']) as $functionality) {
            $rows[$nid][] = render($element['nodes'][$nid]['functionalities'][$functionality]);
        }

        $rows[$nid][] = render($element['nodes'][$nid]['delete']);

    }

    if(!empty($rows)) {
        $headers = array_merge(array(
                t("Title"),
            ),
            $element['##functionalities_labels'],
            array("") // colonne 'delete'
        );

        $output .= theme('table', array('header' => $headers, 'rows' => $rows));
    }

    $rows = array(
        array(
            render($element['add_node']['node']),
            render($element['add_node']['add']),
        ),
    );

    $output .= theme('table', array('header' => array(), 'rows' => $rows,));

    return $output;
}