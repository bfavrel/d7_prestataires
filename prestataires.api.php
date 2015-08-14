<?php

/**
 * Implementation of hook_prestataires_infos()
 * Provides a complete list of functionalities to 'prestataires' module.
 *
 * @return array : indexed by functionality's system name :
 *                      - 'label' : string : human readable functionality's name
 *                      - 'group' : string : config param group : 'nodes'
 *                      - 'filter_callback' : string : function name : this function is intended to return a boolean indicates
 *                                                     if module accepts to process given node/user (true) or reject it (false).
 *                                                     It's called with $nid and $uid parameters.
 */
function hook_prestataires_infos() {}

/**
 * Implementation of hook_prestataires_node_data()
 *
 * Modules can associate some data to a node, in the context of Prestataires modules.
 * This hook is called once, during submission of association node/user form.
 *
 * @param int $nid
 * @param int $uid
 *
 * @return string : serialized array, simple string, (string)int, etc.
 */
function hook_prestataires_node_data($nid, $uid) {}

/**
 * Implementation of hook_prestataires_node_add()
 *
 * Informs modules that a node has been associated.
 *
 * @param int $nid
 * @param int $uid
 */
function hook_prestataires_node_add($nid, $uid) {}