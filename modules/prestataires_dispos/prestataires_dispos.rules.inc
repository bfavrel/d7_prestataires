<?php

function prestataires_dispos_rules_event_info() {
    return array(
        'pdispos_form_submitted' => array(
            'label' => t("After dispos form submission"),
            'group' => t("Prestataires Dispos module"),
            'variables' => array(
                'node' => array(
                    'label' => "Updated Node",
                    'type' => 'node',                    
                ),
            ),
        ),
    );
}