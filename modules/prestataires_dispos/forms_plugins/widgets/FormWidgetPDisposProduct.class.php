<?php

class FormWidgetPDisposProduct extends FormWidgetAbstract
{
    public function widgetConfigurationForm(array $form_state, array $configuration, $lang, array $stored_values, array $available_values = array()) {

        // ce widget possède t-il un formulaire de configuration ?
        if(empty($form_state)) {return true;}

        if(isset($configuration['module_custom']['capacity']) && $configuration['module_custom']['capacity'] > 0) {
            $default_capacity = $configuration['module_custom']['capacity'];
        } else {
            $default_capacity = 0;
        }
        
        $form = array(
            'configuration' => array(
                'capacity' => array(
                    '#type' => 'textfield',
                    '#title' => t("Capacity"),
                    '#description' => t("Must be set to '0' if capacity is not relevant."),
                    '#field_suffix' => t("people@plurial", array('@plurial' => null)),
                    '#size' => 2,
                    '#default_value' =>  $default_capacity,
                ),

                'display_capacity' => array(
                    '#type' => 'checkbox',
                    '#title' => t("Display capacity in label"),
                    '#default_value' => isset($configuration['module_custom']['display_capacity']) ? $configuration['module_custom']['display_capacity'] : 0,// @see submit()
                ),
            ),
        );

        return $form;
    }

    public function widgetConfigurationFormSubmit(array $form, array &$form_state, array &$configuration, array &$stored_values) {
        // dans la mesure ou ce couple field/widget est propre au module 'prestataires_dispos', inutile de s'ambarrasser
        // avec une config par callback. Néanmoins, les valeurs doivent être disponibles pour le module, à la soumissions.
        // on sauvegarde donc ces données en tant que données 'custom'.
        $configuration['module_custom']['capacity'] = $form_state['values']['configuration']['capacity'] > 0 ? $form_state['values']['configuration']['capacity'] : -1;
        $configuration['module_custom']['display_capacity'] = $configuration['module_custom']['capacity'] > 0 ? $form_state['values']['configuration']['display_capacity'] : 0;
    }

    public function render(array $configuration, array $stored_values, array $default_value) {

        $default_values = $default_value + array(
            'quantity' => '0',
            'comment' => "",
        );

        $element = array(
            'quantity' => array(
                '#type' => 'textfield',
                '#title' => t("Qty."),
                '#size' => 2,
                '#default_value' => $default_values['quantity'],
            ),

            'comment' => array(
                '#type' => 'textfield',
                '#title' => t("Comment"),
                '#description' => t("A short remark (max : @num chars)", array('@num' => 128)),
                '#max_length' => 128,
                '#default_value' => $default_values['comment'],
            ),
        );

        return $element;
    }

    public function compileValues(array $raw_value, array $configuration, array $stored_values) {

        if($raw_value['quantity'] == 0 && $raw_value['comment'] == "") {
            return array();
        }

        return $raw_value;
    }
}