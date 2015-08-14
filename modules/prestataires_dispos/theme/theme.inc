<?php


/**
 * Theme the listing infos resulting of a search
 * @see prestataires_dispos_so_listings_node()
 */
function theme_prestataire_dispos_listing_infos($vars) {
    
    $output = "";
        
    $output .= "<div class='pdispos_listing_infos'>";
    
    $output .= "<div class='pdispo_global_update'>" . $vars['global_last_update'] . "</div>";
    
    $output .= "<div class='pdispo_infos'>" . theme('prestataire_dispos_infos', array('infos' => $vars['infos'])) . "</div>";
    
    if($vars['more_infos'] == true) {
        $output .= "<div class='pdispo_more_infos'>" . t("There are additional availabilities. Please, see detailed page.") . "</div>";
    }
    
    $output .= "</div>";
    
    return $output;
}

/**
 * Theme the partially rendered dispos infos
 * @see prestataires_dispos_get_dispo_infos()
 */
function theme_prestataire_dispos_infos($vars) {

    $output = "";

    $infos = $vars['infos'];

    $output .= "<h3>" . $infos['dates'] . "</h3>";

    $output .= "<div class='pdispo_type_label'>" . $infos['type'] . ".</div>";
    $output .= "<div class='pdispo_updated'>" . $infos['updated'] . ".</div>";

    $rows = array();

    foreach($infos['products'] as $product) {
        $rows[] = array(
            "<span class='pdispos_product_label'>" . $product['label'] . "</span>",
            $product['quantity'],
            !empty($product['comment']) ? $product['comment'] : "<em>" . t("No comment") . "</em>",
        );
    }

    if(!empty($rows)) {
        $output .= theme('table', array(
            'header' => array(
                t("Product"),
                t("Remaining"),
                t("Comment"),
            ),
            'rows' => $rows,
            'attributes' => array(
                'class' => array('pdispos_product_infos'),
            ),
        ));
    }

    return $output;
}
