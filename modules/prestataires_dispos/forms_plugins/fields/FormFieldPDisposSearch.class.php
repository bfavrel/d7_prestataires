<?php

class FormFieldPDisposSearch extends FormFieldDates
{
    public static function getCompatibleWidgets(array $field_definition) {
        return array('pdispos_search_widget');
    }

    public function compileUserInputs(array &$raw_value) {// passage par référence car le parent a besoin de pouvoir altérer la valeur
        $user_input = parent::compileUserInputs($raw_value);

        $user_input['values']['capacity'] = array_key_exists('capacity', $raw_value) ? $raw_value['capacity'] : 0;
        $user_input['values']['accuracy'] = $raw_value['accuracy'];

        return $user_input;
    }
}
