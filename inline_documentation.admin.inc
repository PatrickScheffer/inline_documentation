<?php
/**
 * @file
 * Defines the administration forms for managing inline documentation.
 */

/*
 * Form generator functions.
 */

/**
 * List all inline documentation bundles form.
 *
 * Used by the following menu items:
 * <ul>
 *   <li>admin/structure/inline_documentation</li>
 *   <li>admin/structure/inline_documentation/list</li>
 * </ul>
 */
function inline_documentation_form_overview_inline_documentation_bundles($form) {
  // Retrieve all inline documentation bundles.
  $inline_documentation_bundles = inline_documentation_get_inline_documentation_bundles();
  // All form should be rendered as a tree.
  $form['#tree'] = TRUE;
  // Add items container.
  $form['items'] = array();
  // Retrieve inline documentation count.
  $inline_documentation_count = count($inline_documentation_bundles);
  // Iterate through inline documentation_bundles.
  foreach ($inline_documentation_bundles as $inline_documentation) {
    // Retrieve the form element entry by reference.
    $inline_documentation_element = &$form['items'][$inline_documentation->did];
    // Initialize the inline documentation element.
    $inline_documentation_element = array();
    // Add the inline documentation as a property of the form element.
    $inline_documentation_element['#inline_documentation'] = $inline_documentation;
    // Add inline documentation name as a markup element.
    $inline_documentation_element['name'] = array('#markup' => check_plain($inline_documentation->name));
    // Add edit, list and add link.
    $inline_documentation_element['list'] = array(
      '#type' => 'link',
      '#title' => t('list items'),
      '#href' => "inline_documentation/{$inline_documentation->machine_name}",
    );
    $inline_documentation_element['add'] = array(
      '#type' => 'link',
      '#title' => t('add item'),
      '#href' => "inline_documentation/{$inline_documentation->machine_name}/add",
    );
    $inline_documentation_element['edit'] = array(
      '#type' => 'link',
      '#title' => t('edit'),
      '#href' => "admin/structure/inline_documentation/{$inline_documentation->machine_name}/edit",
    );
    $inline_documentation_element['delete'] = array(
      '#type' => 'link',
      '#title' => t('delete'),
      '#href' => "admin/structure/inline_documentation/{$inline_documentation->machine_name}/delete",
    );
  }
  $form['#empty_text'] = t('No inline documentation bundles available. <a href="@link">Add inline documentation bundle</a>.', array('@link' => url('admin/structure/inline_documentation/add')));

  return $form;
}

/**
 * Create or edit an inline documentation bundle form.
 *
 * Used by the following menu items:
 * <ul>
 *   <li>admin/structure/inline_documentation/add</li>
 *   <li>admin/structure/inline_documentation/%inline_documentation_machine_name/edit</li>
 * </ul>
 */
function inline_documentation_form_inline_documentation_bundle($form, &$form_state, $edit = array()) {
  // During initial form build, add the entity to the form state for use
  // during form building and processing. During a rebuild, use what is in
  // the form state.
  if (!isset($form_state['inline_documentation'])) {
    // Convert the edit variable to an object.
    $inline_documentation = is_object($edit) ? $edit : (object) $edit;
    // Create array with default values.
    $default = array(
      'name' => '',
      'machine_name' => '',
      'description' => '',
    );
    // Iterate through default values.
    foreach ($default as $key => $value) {
      // If the property is missing.
      if (!isset($inline_documentation->{$key})) {
        // Then add the default value.
        $inline_documentation->{$key} = $value;
      }
    }
    // Assign inline_documentation to the form state.
    $form_state['inline_documentation'] = $inline_documentation;
  }
  else {
    // Retrieve the inline documentation bundle from the form state.
    $inline_documentation = $form_state['inline_documentation'];
  }
  // Add inline documentation bundle to the form.
  $form['#inline_documentation'] = $form_state['inline_documentation'];
  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#description' => t('The name of the inline documentation bundle is used as title.'),
    '#default_value' => $inline_documentation->name,
    '#maxlength' => 255,
    '#required' => TRUE,
  );
  $form['machine_name'] = array(
    '#type' => 'machine_name',
    '#default_value' => $inline_documentation->machine_name,
    '#maxlength' => 255,
    '#machine_name' => array(
      'exists' => 'inline_documentation_machine_name_load',
    ),
  );
  $form['old_machine_name'] = array(
    '#type' => 'value',
    '#value' => $inline_documentation->machine_name,
  );
  $form['description'] = array(
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#description' => t('The description can be used as an explanatory text.'),
    '#default_value' => $inline_documentation->description,
  );
  // Add supported actions for this form.
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array('#type' => 'submit', '#value' => t('Save'));
  // Return the generated form.
  return $form;
}

/**
 * Confirm inline documentation bundle deletion form.
 *
 * Used by the following menu items:
 * <ul>
 *   <li>admin/structure/inline_documentation/%inline_documentation_machine_name/delete</li>
 * </ul>
 */
function inline_documentation_form_confirm_delete_inline_documentation_bundle($form, &$form_state, $inline_documentation) {
  // Always provide entity id in the same form key as in the entity edit form.
  $form['did'] = array('#type' => 'value', '#value' => $inline_documentation->did);
  // Add inline documentation bundle to the form.
  $form['#inline_documentation'] = $inline_documentation;
  $form['type'] = array('#type' => 'value', '#value' => 'inline_documentation');
  $form['name'] = array('#type' => 'value', '#value' => $inline_documentation->name);
  return confirm_form(
      $form,
      t('Are you sure you want to delete the inline documentation bundle %name?', array('%name' => $inline_documentation->name)),
      'admin/structure/inline_documentation',
      t('Deleting an inline documentation bundle will delete all the inline documentation items in it. This action cannot be undone.'),
      t('Delete'),
      t('Cancel')
  );
}

/*
 * Form validation handlers.
 */

/**
 * Form validation handler for
 * 'inline_documentation_form_inline_documentation_bundle'.
 */
function inline_documentation_form_inline_documentation_bundle_validate($form, &$form_state) {
  // During the deletion there is no 'machine_name' key.
  if (isset($form_state['values']['machine_name'])) {
    // Do not allow machine names to conflict with inline documentation bundle
    // path arguments.
    $machine_name = $form_state['values']['machine_name'];
    // Invalid machine names which will conflict with the inline documentation
    // bundle path arguments.
    $disallowed = array('add', 'list');
    // Check if the machine name has on of the disallowed values.
    if (in_array($machine_name, $disallowed)) {
      // Raise form error.
      form_set_error('machine_name', t('The machine-readable name cannot be "add" or "list".'));
    }
  }
}

/*
 * Form submit handlers.
 */

/**
 * Form submit handler for
 * 'inline_documentation_form_inline_documentation_bundle'.
 */
function inline_documentation_form_inline_documentation_bundle_submit($form, &$form_state) {
  // Retrieve the inline documentation bundle.
  $inline_documentation = $form_state['inline_documentation'];
  // Submit the inline documentation bundle entity.
  entity_form_submit_build_entity('inline_documentation', $inline_documentation, $form, $form_state);
  // Evaluate the save status.
  switch (inline_documentation_save($inline_documentation)) {
    case SAVED_NEW:
      drupal_set_message(t('Created new inline documentation bundle %name.', array('%name' => $inline_documentation->name)));
      watchdog('inline_documentation', 'Created new inline documentation bundle %name.', array('%name' => $inline_documentation->name), WATCHDOG_NOTICE, l(t('edit'), "admin/structure/inline_documentation/{$inline_documentation->machine_name}/edit"));
      break;

    case SAVED_UPDATED:
      drupal_set_message(t('Updated inline documentation bundle %name.', array('%name' => $inline_documentation->name)));
      watchdog('inline_documentation', 'Updated inline documentation bundle %name.', array('%name' => $inline_documentation->name), WATCHDOG_NOTICE, l(t('edit'), "admin/structure/inline_documentation/{$inline_documentation->machine_name}/edit"));
      break;
  }
  // Add inline documentation bundle information to the form state.
  $form_state['values']['did'] = $inline_documentation->did;
  $form_state['did'] = $inline_documentation->did;
  // Redirect to the 'admin/structure/inline_documentation' page.
  $form_state['redirect'] = 'admin/structure/inline_documentation';
}

/**
 * Form submit handler for
 * 'inline_documentation_form_confirm_delete_inline_documentation_bundle'.
 */
function inline_documentation_form_confirm_delete_inline_documentation_bundle_submit($form, &$form_state) {
  // Delete the inline documentation bundle.
  inline_documentation_delete($form_state['values']['did']);
  drupal_set_message(t('Deleted inline documentation bundle %name', array('%name' => $form_state['values']['name'])));
  watchdog('inline_documentation', 'Deleted inline documentation bundle %name', array('%name' => $form_state['values']['name']), WATCHDOG_NOTICE);
  $form_state['redirect'] = 'admin/structure/inline_documentation';
  // Clear the page and block caches to avoid stale data.
  cache_clear_all();
}
