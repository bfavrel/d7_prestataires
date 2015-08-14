<?php

class FormWidgetPDisposSearch extends FormWidgetDatefields
{
    public function widgetConfigurationForm(array $form_state, array $configuration, $lang, array $stored_values, array $available_values = array()) {

        $form = parent::widgetConfigurationForm($form_state, $configuration, $lang, $stored_values, $available_values);

        if(empty($form_state)) {return true;}

        $form['configuration']['capacity_search'] = array(
            '#type' => 'checkbox',
            '#title' => t("Permit search by capacity"),
            '#default_value' => isset($configuration['module_custom']['capacity_search']) ? $configuration['module_custom']['capacity_search'] : 0,
            '#weight' => 10,
        );

        $form['configuration']['undefined_mode'] = array(
            '#type' => 'radios',
            '#title' => t("\"Undefined availabilities\" behavior"),
            '#options' => array(
                0 => t("must be treated as invalid"),
                1 => t("must be treated as valid"),
            ),
            '#default_value' => isset($configuration['module_custom']['undefined_mode']) ? $configuration['module_custom']['undefined_mode'] : 0,
            '#weight' => 11,
        );

        $form['configuration']['shift_dates'] = array(
            '#type' => 'checkboxes',
            '#title' => t("Dates offsets"),
            '#description' => t("Will be discarded if period between dates isn't large enough."),
            '#options' => array(
                'start' => t("Search on start date +1 day"),
                'end' => t("Search on end date -1 day"),
            ),
            '#default_value' => (array)$configuration['module_custom']['shift_dates'],
            '#weight' => 12,
        );

        return $form;
    }

    public function widgetConfigurationFormSubmit(array $form, array &$form_state, array &$configuration, array &$stored_values) {

        parent::widgetConfigurationFormSubmit($form, $form_state, $configuration, $stored_values);

        // dans la mesure ou ce couple field/widget est propre au module 'prestataires_dispos', inutile de s'embarrasser
        // avec une config par callback. Néanmoins, les valeurs doivent être disponibles pour le module, à la soumissions.
        // on sauvegarde donc ces données en tant que données 'custom'.

        $configuration['module_custom']['capacity_search'] = $form_state['values']['configuration']['capacity_search'];

        $configuration['module_custom']['undefined_mode'] = $form_state['values']['configuration']['undefined_mode'];

        $configuration['module_custom']['shift_dates'] = $form_state['values']['configuration']['shift_dates'];
    }

    public function render(array $configuration, array $stored_values, array $default_value) {

        // si les valeurs ont été  passées dans l'URL, elles sont au format "array" et non "string"
        array_walk($default_value, function(&$val){
            $val = is_array($val) ? $val[0] : $val;
        });

        $element = array();

        $element['from'] = array(
            '#type' => 'date_popup',
            '#date_format' => 'd/m/Y',
            '#date_year_range' => '0:+1',
            '#title' => $configuration['field_from_label'],
            '#title_display' => 'invisible',
            '#default_value' => $default_value['from'],
            '#process' => array('prestataires_dispos_process_date_popup'),
            '#weight' => 0,
        );

        $element['to'] = array(
            '#type' => 'date_popup',
            '#date_format' => 'd/m/Y',
            '#date_year_range' => '0:+1',
            '#title' => $configuration['field_to_label'],
            '#title_display' => 'invisible',
            '#default_value' => $default_value['to'],
            '#process' => array('prestataires_dispos_process_date_popup'),
            '#weight' => 1,
        );

        if($configuration['module_custom']['capacity_search'] == true) {
            $element['capacity'] = array(
                '#type' => 'textfield',
                '#title' => null,
                '#field_prefix' => t("For@npeople", array('@npeople' => null)),
                '#field_suffix' => t("people@optionalplurial", array('@optionalplurial' => null)),
                '#size' => 1,
                '#default_value' => $default_value['capacity'],
                '#attributes' => array('placeholder' => "?"),
                '#weight' => 10,
            );
        }

        $element['accuracy'] = array(
            '#type' => 'checkbox',
            '#title' => t("Consecutive availabilities in the whole period"),
            '#default_value' => $default_value['accuracy'],
            '#weight' => 20,
        );

        return $element;
    }
}