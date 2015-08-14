<?php

class FormFieldPDisposProduct extends FormFieldAbstract
{
    public static function getCompatibleWidgets(array $field_definition) {
        return array('pdispos_product');
    }

    public function fieldConfigurationForm(array $form_state) {

        // ce widget possède t-il un formulaire de configuration ?
        if(empty($form_state)) {
            return true;
        }

        $form = array(
            '#tree' => true,
            '#submit' => array('so_forms_edit_field_submit'), // obligatoire afin d'avoir les submit callbacks dans le bon ordre

            'configuration' => array(
                'module_custom' => $this->executeCallback($this->_configuration_callback),
            ),
        );

        // si le module souhaite intervenir sur la soumission de ses propres éléments de form
        // TODO : dangereux : le module a accès à tous les paramètres : c'est la porte ouverte au bricolage. A corriger sous D7.
        array_merge($form['#submit'], (array)$form['configuration']['module_custom']['#submit']);
        unset($form['configuration']['module_custom']['#submit']);

        $form = array_merge_recursive($form, (array)$this->_widget->widgetConfigurationForm($form_state, $this->_configuration, $this->_language, $this->_stored_values));

        return $form;
    }

    public function fieldConfigurationFormSubmit(array $form, array &$form_state) {

        if(!empty($form_state['values']['configuration']['module_custom'])) {
            $this->_configuration['module_custom'] = $form_state['values']['configuration']['module_custom'];
        }

        $this->_disable = $this->_widget->widgetConfigurationFormSubmit($form, $form_state, $this->_configuration, $this->_stored_values);
    }

    public function render(array $default_value) {

        $element = array(
            '#type' => 'container',
            '#weight' => $this->_weight,
            '#attributes' => array('class' => array('clearfix')),

            'label' => array(
                '#prefix' => "<div class='pdispos_product_label'>",
                '#markup' => t($this->_label) .
                            ($this->_configuration['module_custom']['display_capacity'] == true ? " (" . format_plural(
                                    $this->_configuration['module_custom']['capacity'],
                                    "for 1 people",
                                    "for @count people"
                            ) . ")" : "") . " : ",
                '#suffix' => "</div>",
            ),
        );

        $element = array_merge($element, (array)$this->_widget->render($this->_configuration, $this->_stored_values, $default_value));

        if(!empty($this->_render_callback)) {
            // à cause de func_get_args() (copie des argument), il n'est malheureusement pas possible d'altérer l'élément par référence.
            $element = $this->executeCallback($this->_render_callback, $element);
        }

        return $element;
    }

    public function compileUserInputs(array &$raw_value) {

        $user_input = parent::compileUserInputs($raw_value);
        $user_input = array_merge($user_input, array('values' => $this->_widget->compileValues($raw_value, $this->_configuration, $this->_stored_values)));

        return $user_input;
    }
}