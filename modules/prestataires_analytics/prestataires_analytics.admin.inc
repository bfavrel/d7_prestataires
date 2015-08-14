<?php

function prestataires_analytics_settings_form($form, &$form_state) {

    if(prestataires_analytics_load_gapi() == false) {return;}

    $settings = variable_get('prestataires_analytics', array(
        'account' => array(),
        'cron' => array(),
        'chart' => array(),
        'stats' => array(),
    ));

    // nettoyage suite au passage à OAuth 2.0
    if(array_key_exists('id_site', $settings['account'])) {
        $settings['account'] = array();
        variable_set('prestataires_analytics', $settings);
    }

    $last_cron = variable_get('prestataires_analytics_last_cron', 0);

    if($last_cron > 0) {
        $last_cron_date = DateTime::createFromFormat('U', $last_cron);
        $last_cron_date->setTimezone(new DateTimeZone("Europe/Paris"));
        $last_cron_msg = t("Last update : @date at @hour", array('@date' => $last_cron_date->format('d/m/Y'), '@hour' => $last_cron_date->format('H:i:s')));
    } else {
        $last_cron_msg = "";
    }

    if(array_key_exists('account', $settings) && !empty($settings['account'])) {
        $account_mail = $settings['account']['account_mail'];
        $certificate_fid = $settings['account']['certificate'];
        $profile_id = $settings['account']['profile_id'];
    }

    $form = array(
        '#tree' => true,

        'account' => array(
            '#type' => 'container',
            '#id' => 'account-wrapper',

            '#theme_wrappers' => array('fieldset'),
            '#title' => t("Account"),

            'account_mail' => array(
                '#type' => 'textfield',
                '#title' => t("Account mail address"),
                '#description' => t("Address in the form \"<strong>...@developer.gserviceaccount.com</strong>\"."),
                '#default_value' => $account_mail,
                '#size' => 80,
                '#required' => true,
            ),

            'profile_id' => array(
                '#type' => 'textfield',
                '#title' => t("Profile ID"),
                '#description' => t("A \"[0-9]+\" string."),
                '#default_value' => $profile_id,
                '#required' => true,
            ),

            'certificate' => array(
                '#type' => 'managed_file',
                '#title' => t("P12 certificate"),
                '#upload_location' => 'private://prestataires_analytics/',
                '#upload_validators' => array(
                    'file_validate_extensions' => array('p12'),
                ),
                '#default_value' => $certificate_fid,
                '#required' => true,
            ),
            'certificate_fid' => array(
                '#type' => 'value',
                '#value' => $certificate_fid,
            ),

            'update_account' => array(
                '#type' => 'submit',
                '#value' => t("Update and test"),
                '#ajax' => array(
                    'callback' => 'prestataires_analytics_settings_form_callback',
                    'wrapper' => 'account-wrapper',
                    'effect' => 'none',
                    'event' => 'click',
                    'method' => 'replace',
                    'progress' => array(
                        'type' => 'throbber',
                        'message' => null,
                    ),
                ),
            ),
        ),

        'cron' => array(
            '#type' => 'container',

            '#theme_wrappers' => array('fieldset'),
            '#title' => t("Cron"),

            'time_windows' => array(
                '#type' => 'textfield',
                '#title' => t("Time window(s) for cron"),
                '#description' => t("Two hours (separated by '-') periods, separated by '|'.<br />Exemple : '0-6|13-14' = \"one time between 00h00 (included) and 06h00 (not included) and one time between 13h (included) and 14h (not included)\"."),
                '#default_value' => !empty($settings['cron']) ? $settings['cron']['time_windows'] : '0-6',
                '#size' => 25,
                '#required' => true,
            ),

            'launch' => array(
                '#type' => 'submit',
                '#value' => t("Launch update NOW !"),
                '#submit' => array('prestataires_analytics_settings_form_cron_submit'),
            ),

            'last_update' => array(
                '#markup' => $last_cron_msg,
            ),
        ),

        'chart' => array(
            '#type' => 'container',

            '#theme_wrappers' => array('fieldset'),
            '#title' => t("Chart"),

            'chart_color' => array(
                '#type' => 'textfield',
                '#title' => t("Chart color"),
                '#description' => t("Format '#rrggbb'"),
                '#default_value' => !empty($settings['chart']) ? $settings['chart']['chart_color'] : '#ff0000',
                '#size' => 20,
                '#required' => true,
            ),

            'font_color' => array(
                '#type' => 'textfield',
                '#title' => t("Font color"),
                '#description' => t("Format '#rrggbb'"),
                '#default_value' => !empty($settings['chart']) ? $settings['chart']['font_color'] : '#666666',
                '#size' => 20,
                '#required' => true,
            ),
        ),
        'stats' => array(
            '#type' => 'container',

            '#theme_wrappers' => array('fieldset'),
            '#title' => t("Stats"),

            'config' => array(
                '#type' => 'textarea',
                '#description' => t("See module's 'default_stats.ini' for documentation."),
                '#title' => null,
                '#default_value' => !empty($settings['stats'])
                                    ? $settings['stats']['config']
                                    : file_get_contents(drupal_get_path('module', 'prestataires_analytics') . '/default_stats.ini'),
                '#rows' => 30
            ),

            'debug' => array(
                '#type' => 'checkbox',
                '#title' => t("Display debug informations"),
                '#default_value' => !empty($settings['stats'])
                                    ? $settings['stats']['debug']
                                    : 0,
            ),
        ),

        'save' => array(
            '#type' => 'submit',
            '#value' => t("Save"),
        ),
    );

    return $form;
}

function prestataires_analytics_settings_form_callback($form, &$form_state) {
    return $form['account'];
}

function prestataires_analytics_settings_form_cron_submit($form, &$form_state) {
    prestataires_analytics_cron(true);
    drupal_set_message("Update finished.");
}

function prestataires_analytics_settings_form_submit($form, &$form_state) {

    $settings = variable_get('prestataires_analytics', array());
    $op = array_pop($form_state['triggering_element']['#parents']);

    if($op == 'update_account') {

        $file = file_load($form_state['values']['account']['certificate']);

        // nouveau certificat
        if($form_state['values']['account']['certificate'] != $form_state['values']['account']['certificate_fid']) {

            $file->status = FILE_STATUS_PERMANENT;
            file_save($file);
            file_usage_add($file, 'prestataires_analytics', 'certificate', 1);

            $settings['account']['certificate'] = $form_state['values']['account']['certificate'];

            // certificat obsolete
            if(!empty($form_state['values']['account']['certificate_fid'])) {
                $old_file = file_load($form_state['values']['account']['certificate_fid']);
                file_usage_delete($old_file, 'prestataires_analytics', 'certificate', 1);
                file_delete($old_file, false);
            }
        }

        if(prestataires_analytics_load_gapi() != false) {

            try {
                new gapi($form_state['values']['account']['account_mail'], $file->uri);
            } catch (Exception $e) {
                drupal_set_message(t("An error has occured : \"@msg\"", array('@msg' => $e->getMessage())), 'error');
            }

            if(empty($e)) {
                drupal_set_message(t("All is ok. Module communicates with GGA service.<br />Don't forget to save the settings."));
            }
        }

        $form_state['rebuild'] = true;

    } else { // 'save'

        unset($form_state['values']['account']['certificate_fid']);
        unset($form_state['values']['account']['update_account']);

        $settings['account'] = array_merge((array)$settings['account'], $form_state['values']['account']);

        unset($form_state['values']['cron']['launch']);
        $settings['cron'] = $form_state['values']['cron'];

        $settings['chart'] = $form_state['values']['chart'];
        $settings['stats'] = $form_state['values']['stats'];

        drupal_set_message(t("Configuration has been saved"));
    }

    variable_set('prestataires_analytics', $settings);
}

function prestataires_analytics_user_admin_form($form, &$form_state, $account) {

    $uid = $account->uid;

    $settings = variable_get('prestataires_analytics', array());
    $stats_configs = parse_ini_string($settings['stats']['config'], true);

    $form = array(
        '#tree' => true,

        'nodes' => array(),

        'save' => array(
            '#type' => 'submit',
            '#value' => t("Save"),
        ),
    );

    $results = db_query(
        "SELECT p.nid, p.params, d.data, n.type, n.title FROM {prestataires} AS p
        LEFT JOIN {prestataires_nodes_data} AS d ON d.nid=p.nid AND (d.module='prestataires_analytics' OR d.module IS NULL)
        LEFT JOIN {node} AS n ON n.nid=p.nid
        WHERE p.uid=? AND n.nid IS NOT NULL",
        array($uid)
    );

    while($row = $results->fetchObject()) {

        $row->params = unserialize($row->params);

        if(!array_key_exists('analytics@prestataires_analytics', $row->params['functionalities'])
            || $row->params['functionalities']['analytics@prestataires_analytics'] != true) {continue;}

        $stored_data = prestataires_get_node_data($row->nid, 'prestataires_analytics');

        $row->data = array_filter((array)unserialize($row->data));

        $form['nodes'][$row->nid] = array(
            '#type' => 'fieldset',
            '#title' => $row->title . " (" . node_type_get_name($row->type) . ")",
            '#collapsible' => true,
            '#collapsed' => true,
        );

        $fields = array();

        foreach($stats_configs as $stat_config) {

            if($stat_config['user_params'] != true) {continue;}

            $fields = array_merge($fields, _prestataires_analytics_extract_exposed_fields($stat_config['expose']));

            $parsed_filter = _prestataires_analytics_extract_fields_from_filter($stat_config['filter']);

            foreach($parsed_filter as $member) {
                $fields = array_merge($fields, $member[2]);
            }
        }

        array_walk($fields, function(&$val) {
            if(strpos($val, '#') === 0) {
                $val = null;
            }
        });

        $fields = array_filter(array_unique($fields));

        foreach($fields as $fieldname) {

            $instance = field_info_instance('node', $fieldname, $row->type);

            $form['nodes'][$row->nid][$fieldname] = array(
                '#type' => 'textfield',
                '#title' => $instance['label'] . " (" . $fieldname . ")",
                '#default_value' => $stored_data['additional_values'][$fieldname],
            );
        }
    }

    return $form;
}

function prestataires_analytics_user_admin_form_submit($form, &$form_state) {

    foreach($form_state['values']['nodes'] as $nid => $fields) {

        $data = prestataires_get_node_data($nid, 'prestataires_analytics');
        $data['additional_values'] = array_filter($fields);

        prestataires_store_node_data($nid, 'prestataires_analytics', $data);
    }

    drupal_set_message(t("Configuration has been saved"));
}