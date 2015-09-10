<?php

function prestataires_dispos_settings() {

    $output = array();

    drupal_set_title("Prestataires dispos");

    $output[] = drupal_get_form('prestataires_dispos_general_settings_form');

    $output[] = drupal_get_form('prestataires_dispos_nodes_types_settings_form');

    return $output;
}

function prestataires_dispos_general_settings_form($form, &$form_state) {

    $settings = variable_get('prestataires_dispos_settings', array(
        'configuration' => array(),
        'nodes_types' => array(),
    ));

    $results = db_query("SELECT DISTINCT layer FROM {prestataires_dispos} WHERE layer NOT IN(0,255) ORDER BY layer ASC");

    $layers_options = array();

    while($row = $results->fetchObject()) {
        $layers_options[$row->layer] = t("layer") . " " . $row->layer;
    }

    ksort($layers_options);

    if(empty($layers_options)) {
        $layers_description = t("No modules' layer available");
    } else {
        $layers_description = "";
    }

    return array(
        '#theme_wrappers' => array('form', 'fieldset'),
        '#title' => t("General settings"),

        'display_irrelevant_results_infos' => array(
            '#type' => 'checkbox',
            '#title' => t("Display irrelevant results infos anyway"),
            '#default_value' => array_key_exists('display_irrelevant_results_infos', $settings['configuration']) ? $settings['configuration']['display_irrelevant_results_infos'] : false,
        ),

        'hide_dispos_sfa_parents' => array(
            '#type' => 'checkbox',
            '#title' => t("Don't display availabilities for SFA parents (if module enabled)"),
            '#default_value' => array_key_exists('hide_dispos_sfa_parents', $settings['configuration']) ? $settings['configuration']['hide_dispos_sfa_parents'] : false,
        ),

        'debug_mode' => array(
            '#type' => 'checkbox',
            '#title' => t("Debug mode"),
            '#default_value' => array_key_exists('debug_mode', $settings['configuration']) ? $settings['configuration']['debug_mode'] : false,
        ),

        'layers_filter' => array(
            '#type' => 'checkboxes',
            '#title' => t("Modules' layers to display in nodes"),
            '#description' => $layers_description,
            '#options' => $layers_options,
            '#default_value' => array_key_exists('layers_filter', $settings['configuration']) ? $settings['configuration']['layers_filter'] : array(),
        ),

        'save' => array(
            '#type' => 'submit',
            '#value' => t("Save"),
        ),
    );
}

function prestataires_dispos_general_settings_form_submit($form, &$form_state) {

    $settings = variable_get('prestataires_dispos_settings', array(
        'configuration' => array(),
        'nodes_types' => array(),
    ));

    $settings['configuration']['hide_dispos_sfa_parents'] = $form_state['values']['hide_dispos_sfa_parents'];
    $settings['configuration']['debug_mode'] = $form_state['values']['debug_mode'];
    $settings['configuration']['display_irrelevant_results_infos'] = $form_state['values']['display_irrelevant_results_infos'];
    $settings['configuration']['layers_filter'] = array_filter($form_state['values']['layers_filter']);

    variable_set('prestataires_dispos_settings', $settings);

    drupal_set_message(t("Parameters have been saved"));
}

function prestataires_dispos_nodes_types_settings_form($form, &$form_state) {

    drupal_set_title("Prestataires dispos");

    $settings = variable_get('prestataires_dispos_settings', array(
        'configuration' => array(),
        'nodes_types' => array(),
    ));

    $nodes_types_settings = $settings['nodes_types'];
    asort($nodes_types_settings);

    $nodes_types_pool = node_type_get_types();
    $nodes_types_options = array();

    foreach($nodes_types_pool as $node_type) {
        $nodes_types_options[$node_type->type] = $node_type->name;
    }

    $nodes_types_options = array_diff_key($nodes_types_options, $nodes_types_settings);

    $form = array(
        '#tree' => true,

        '#theme_wrappers' => array('form', 'fieldset'),
        '#title' => t("Node types settings"),

        'nodes_types_overview' => array(
            '#type' => 'container',
            '#theme' => 'prestataires_dispos_nodes_types_overview',

            'nodes_types' => array(),

            'add_node_type' => array(

                'node_type' => array(
                    '#type' => 'select',
                    '#title' => t("Node type"),
                    '#options' => $nodes_types_options,
                ),

                'add' => array(
                    '#type' => 'submit',
                    '#value' => t("Add"),
                ),
            ),
        ),
    );

    foreach($nodes_types_settings as $node_type => $infos) {

        $form['nodes_types_overview']['nodes_types'][$node_type] = array(

            'label' => array(
                '#markup' => $nodes_types_pool[$node_type]->name,
            ),

            'operations' => array(
                '#markup' => l(t("edit form"), 'admin/so_forms/configure/prestataires_dispos/' . $node_type) . " " .
                             l(t("delete"), 'admin/config/prestataires/settings/dispos/' . $node_type . '/delete', array('query' => drupal_get_destination())),
            ),
        );
    }

    return $form;
}

function prestataires_dispos_nodes_types_settings_form_submit($form, &$form_state) {

    $settings = variable_get('prestataires_dispos_settings', array(
        'configuration' => array(),
        'nodes_types' => array(),
    ));

    $new_node_type = $form_state['values']['nodes_types_overview']['add_node_type']['node_type'];
    $nodes_types_pool = node_type_get_types();

    $form_config = array(
        'name' => t("Dispos form data for '@nodetype'", array('@nodetype' => $nodes_types_pool[$new_node_type]->name)),
        'memorize' => false,
        'advanced' => false,
        'api_only' => false,
        'no_groups' => false,
        'back_path' => 'admin/config/prestataires/settings/dispos',
        'buttons' => array(
            'submit' => null,
            'reset' => null,
        ),
        'perms' => array(
            'configure' => 'configure prestataires dispos',
            'display' => 'access content',
        ),
        'callbacks' => array(
            'process' => null,
            'populate' => 'prestataires_dispos_form_infos_populate_callback',
            'redirect' => null,
        ),
    );

    $form = so_forms_create('prestataires_dispos', $new_node_type, $form_config);

    $settings['nodes_types'][$new_node_type] = array(
        'node_type' => $new_node_type,
        'form_id' => $form['secured_sfid'],
    );

    variable_set('prestataires_dispos_settings', $settings);

    drupal_set_message(t("Configuration has been saved"));
}

function prestataires_dispos_nodes_types_settings_delete_confirm($form, &$form_state, $node_type) {

    return confirm_form(
        $form,
        t("Are you sure you want to delete dispos settings for '@nodetype' ?", array('@nodetype' => $node_type)),
        null,
        null,
        t("Delete")
    );

}

function prestataires_dispos_nodes_types_settings_delete_confirm_submit($form, &$form_state) {

    $node_type = $form_state['build_info']['args'][0];

    $settings = variable_get('prestataires_dispos_settings', array());

    // DEV : pour le moment, on se contente de supprimer le forms! à voir pour un netoyage plus complet
    so_forms_destroy(null, $settings['nodes_types'][$node_type]['form_id']);

    unset($settings['nodes_types'][$node_type]);
    variable_set('prestataires_dispos_settings', $settings);

    drupal_set_message(t("'@nodetype' has been removed from Prestataires Dispos", array('@nodetype' => $node_type)));
}

function theme_prestataires_dispos_nodes_types_overview($vars) {

    $output = "";

    $rows = array();

    $element = $vars['element'];

    $nodes_types = element_children($element['nodes_types']);

    foreach($nodes_types as $node_type) {

        $rows[] = array(
            render($element['nodes_types'][$node_type]['label']),
            render($element['nodes_types'][$node_type]['operations']),
        );
    }

    if(!empty($rows)) {
        $headers = array(
            t("Node type"),
            t("Operations"),
        );

        $output .= theme('table', array('header' => $headers, 'rows' => $rows));
    }

    $rows = array(
        array(
            render($element['add_node_type']['node_type']),
            render($element['add_node_type']['add']),
        ),
    );

    $output .= theme('table', array('header' => array(), 'rows' => $rows,));

    return $output;
}