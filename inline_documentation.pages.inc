<?php
/**
 * @file
 * Defines the user forms and pages for managing inline documentation items.
 */

/*
 * Page callback functions.
 */

/**
 * Inline_documentation item overview page.
 *
 * Used by the following menu items:
 * <ul>
 *   <li>inline_documentation/%inline_documentation_machine_name/item/%inline_documentation_item/list</li>
 * </ul>
 */
function inline_documentation_redirect_to_inline_documentation_item_overview($inline_documentation_machine_name) {
  drupal_goto("inline_documentation/{$inline_documentation_machine_name}");
}

/*
 * Form generator functions.
 */

/**
 * Form generator function for the page
 * 'inline_documentation/%inline_documentation_machine_name'.
 */
function inline_documentation_form_overview_inline_documentation_items($form, &$form_state, $inline_documentation) {
  $form['#inline_documentation'] = $inline_documentation;
  $form['#tree'] = TRUE;
  // Retrieve the inline documentation tree.
  $tree = inline_documentation_get_tree($inline_documentation->did);
  // Add items container.
  $form['items'] = array();
  // Iterate through the tree.
  foreach ($tree as $tree_item) {
    // Retrieve element by reference.
    $element = &$form['items'][$tree_item->iid];
    // Initialize element.
    $element = array('#inline_documentation_item' => $tree_item);
    $element['view'] = array('#markup' => filter_xss_admin($tree_item->name));
    $element['iid'] = array('#type' => 'hidden', '#value' => $tree_item->iid);
    // Only add the operations which the user has access to.
    if (inline_documentation_user_access('edit', $inline_documentation)) {
      $element['edit'] = array(
        '#type' => 'link',
        '#title' => t('edit'),
        '#href' => "inline_documentation/{$inline_documentation->machine_name}/item/{$tree_item->iid}",
      );
    }
    else {
      $element['edit'] = array();
    }
    // Only add the operations which the user has access to.
    if (inline_documentation_user_access('delete', $inline_documentation)) {
      $element['delete'] = array(
        '#type' => 'link',
        '#title' => t('delete'),
        '#href' => "inline_documentation/{$inline_documentation->machine_name}/item/{$tree_item->iid}/delete",
      );
    }
    else {
      $element['delete'] = array();
    }
  }
  $form['#empty_text'] = t('No items available. <a href="@link">Add item</a>.', array('@link' => url("inline_documentation/{$inline_documentation->machine_name}/add")));
  // Allow user to modify the inline documentation name and description of the
  // inline documentation.
  $form['inline_documentation'] = array(
    '#type' => 'fieldset',
    '#title' => t('Inline documentation information'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['inline_documentation']['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#description' => t('The name of the inline documentation is used as title.'),
    '#default_value' => $inline_documentation->name,
    '#maxlength' => 255,
    '#required' => TRUE,
  );
  $form['inline_documentation']['description'] = array(
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#description' => t('The description can used as an explanatory text.'),
    '#default_value' => $inline_documentation->description,
  );
  // Add the supported actions for this form.
  $form['actions'] = array('#type' => 'actions', '#tree' => TRUE);
  $form['actions']['submit'] = array('#type' => 'submit', '#value' => t('Save'));
  return $form;
}

/**
 * Create or edit form for an inline documentation item.
 *
 * Used by the following menu items:
 * <ul>
 *   <li>inline_documentation/%inline_documentation_machine_name/add</li>
 *   <li>inline_documentation/%inline_documentation_machine_name/item/%inline_documentation_item</li>
 * </ul>
 */
function inline_documentation_form_inline_documentation_item($form, &$form_state, $edit = array(), $inline_documentation = NULL) {
  // During initial form build, add the inline documentation item entity to the
  // form state for use during form building and processing. During a rebuild,
  // use what is in the form state.
  if (!isset($form_state['inline_documentation_item'])) {
    // Convert the edit variable to an object.
    $inline_documentation_item = is_object($edit) ? $edit : (object) $edit;
    // If the inline documentation argument is not set and the inline
    // documentation item has an did.
    if (!isset($inline_documentation) && isset($inline_documentation_item->did)) {
      // Load the inline documentation.
      $inline_documentation = inline_documentation_load($inline_documentation_item->did);
    }
    // Create default values array.
    $defaults = array(
      'iid' => NULL,
      'did' => (isset($inline_documentation) ? $inline_documentation->did : NULL),
      'name' => '',
      'description' => '',
      'element' => '',
      'inline_documentation_machine_name' => (isset($inline_documentation) ? $inline_documentation->machine_name : NULL),
    );
    // Iterate through defaults.
    foreach ($defaults as $key => $value) {
      // If the property is not set then add the default value.
      if (!isset($inline_documentation_item->{$key})) {
        $inline_documentation_item->{$key} = $value;
      }
    }
    // Save the inline documentation item to the form state.
    $form_state['inline_documentation_item'] = $inline_documentation_item;
  }
  else {
    // Retrieve the inline documentation item from the form state.
    $inline_documentation_item = $form_state['inline_documentation_item'];
  }

  // Build the form.
  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#description' => t('Internal name of the inline documentation item. Only administrators will see the name and serves as a visual aid in constructing the inline documentation structure. Therefore make use of meaningful names e.g. "Resource Management".'),
    '#default_value' => $inline_documentation_item->name,
    '#maxlength' => 255,
    '#required' => TRUE,
    '#weight' => -5,
  );
  $form['description'] = array(
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#description' => t('Contains the actual documentation.'),
    '#default_value' => $inline_documentation_item->description,
    '#required' => TRUE,
    '#weight' => -4,
  );
  $form['element'] = array(
    '#type' => 'textfield',
    '#title' => t('Element'),
    '#description' => t('ID of the element this documentation should be attached to.'),
    '#default_value' => $inline_documentation_item->element,
    '#maxlength' => 255,
    '#required' => FALSE,
    '#weight' => -3,
  );
  $form['inline_documentation_machine_name'] = array(
    '#type' => 'value',
    '#value' => (isset($inline_documentation_item->inline_documentation_machine_name) ? $inline_documentation_item->inline_documentation_machine_name : $inline_documentation->machine_name),
  );
  // Add fields to the form.
  field_attach_form('inline_documentation_item', $inline_documentation_item, $form, $form_state);
  $form['did'] = array(
    '#type' => 'value',
    '#value' => $inline_documentation->did,
  );
  $form['iid'] = array(
    '#type' => 'value',
    '#value' => $inline_documentation_item->iid,
  );
  // Add actions.
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#weight' => 5,
  );
  // Redirect the this page.
  $form_state['redirect'] = $_GET['q'];
  // Return the generated form.
  return $form;
}

/**
 * Delete inline documentation item confirm form.
 *
 * Used by the following menu items:
 * <ul>
 *   <li>inline_documentation/%inline_documentation_machine_name/item/%inline_documentation_item/delete</li>
 * </ul>
 */
function inline_documentation_form_confirm_delete_inline_documentation_item($form, &$form_state, $inline_documentation_item) {
  // Always provide entity id in the same form key as in the entity edit form.
  $form['iid'] = array('#type' => 'value', '#value' => $inline_documentation_item->iid);
  // Add inline documentation to the form.
  $form['#inline_documentation_item'] = $inline_documentation_item;
  $form['inline_documentation_machine_name'] = array('#type' => 'value', '#value' => $inline_documentation_item->inline_documentation_machine_name);
  $form['type'] = array('#type' => 'value', '#value' => 'inline_documentation_item');
  $form['name'] = array('#type' => 'value', '#value' => $inline_documentation_item->name);
  return confirm_form(
      $form,
      t('Are you sure you want to delete the inline documentation item %name?', array('%name' => $inline_documentation_item->name)),
      "inline_documentation/{$inline_documentation_item->inline_documentation_machine_name}/item/{$inline_documentation_item->iid}",
      t('This action cannot be undone.'),
      t('Delete'),
      t('Cancel')
  );
}

/*
 * Form validation handlers.
 */

/**
 * Form validation handler for
 * 'inline_documentation_form_inline_documentation_item'.
 */
function inline_documentation_form_inline_documentation_item_validate($form, &$form_state) {
  // Allow the entity fields to be validated.
  entity_form_field_validate('inline_documentation_item', $form, $form_state);
}

/*
 * Form submit handlers.
 */

/**
 * Form submit handler for
 * 'inline_documentation_form_overview_inline_documentation_items'.
 */
function inline_documentation_form_overview_inline_documentation_items_submit($form, &$form_state) {
  // Retrieve the inline documentation bundle which was used to build the form.
  $inline_documentation = $form['#inline_documentation'];
  // Update the name and description for the inline documentation.
  db_update('inline_documentation_data')
    ->fields(array('name' => $form_state['values']['inline_documentation']['name'], 'description' => $form_state['values']['inline_documentation']['description']))
    ->condition('did', $inline_documentation->did, '=')
    ->execute();

  drupal_set_message(t('The configuration options have been saved.'));
}

/**
 * Form submit handler for
 * 'inline_documentation_form_inline_documentation_item'.
 */
function inline_documentation_form_inline_documentation_item_submit($form, &$form_state) {
  // Retrieve the inline documentation item.
  $inline_documentation_item = $form_state['inline_documentation_item'];
  // Submit the entity fields.
  entity_form_submit_build_entity('inline_documentation_item', $inline_documentation_item, $form, $form_state);
  // Evaluate the save operation.
  switch (inline_documentation_item_save($inline_documentation_item)) {
    case SAVED_NEW:
      drupal_set_message(t('Created new inline documentation item %name.', array('%name' => $inline_documentation_item->name)));
      watchdog(
          'inline_documentation',
          'Created new inline documentation item %name',
          array(
            '%name' => $inline_documentation_item->name,
          ),
          WATCHDOG_NOTICE,
          l(t('edit'), "inline_documentation/item/{$inline_documentation_item->iid}/edit"));
      break;

    case SAVED_UPDATED:
      drupal_set_message(t('Updated inline documentation item %name.', array('%name' => $inline_documentation_item->name)));
      watchdog(
          'inline_documentation',
          'Updated inline documentation item %name',
          array(
            '%name' => $inline_documentation_item->name,
          ),
          WATCHDOG_NOTICE,
          l(t('edit'), "inline_documentation/item/{$inline_documentation_item->iid}/edit"));
      // Clear the page and block caches to avoid stale data.
      cache_clear_all();
      break;
  }
  $form_state['values']['iid'] = $inline_documentation_item->iid;
  $form_state['iid'] = $inline_documentation_item->iid;
}

/**
 * Form submit handler for
 * 'inline_documentation_form_confirm_delete_inline_documentation_item'.
 */
function inline_documentation_form_confirm_delete_inline_documentation_item_submit($form, &$form_state) {
  // Delete the inline documentation item.
  inline_documentation_item_delete($form_state['values']['iid']);
  drupal_set_message(t('Deleted inline documentation item %name', array('%name' => $form_state['values']['name'])));
  watchdog('inline_documentation', 'Deleted inline documentation item %name', array('%name' => $form_state['values']['name']), WATCHDOG_NOTICE);
  $form_state['redirect'] = "inline_documentation/{$form_state['values']['inline_documentation_machine_name']}";
  // Clear the page and block caches to avoid stale data.
  cache_clear_all();
}
