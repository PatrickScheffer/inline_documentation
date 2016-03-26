<?php
/**
 * @file
 * Defines the theme functions used by the inline_documentation module.
 */

/**
 * Theme function for the form
 * 'inline_documentation_form_overview_inline_documentation_bundles'.
 */
function theme_inline_documentation_form_overview_inline_documentation_bundles($variables) {
  // Retrieve the form element.
  $form = $variables['form'];
  // Initialize rows array.
  $rows = array();
  // Iterate through form child elements.
  foreach (element_children($form['items']) as $key) {
    // Retrieve form element by reference.
    $child_element = &$form['items'][$key];
    // Only render row if the name element is set.
    if (isset($child_element['name'])) {
      // Initiliaze row variable.
      $row = array();
      $row[] = drupal_render($child_element['name']);
      // Render operation links.
      $row[] = drupal_render($child_element['list']);
      $row[] = drupal_render($child_element['add']);
      $row[] = drupal_render($child_element['edit']);
      $row[] = drupal_render($child_element['delete']);
      // Add row to the rows array.
      $rows[] = array('data' => $row);
    }
  }
  // Construct table header.
  $header = array(t('Inline documentation bundle name'));
  // Add operation header.
  $header[] = array('data' => t('Operations'), 'colspan' => '4');
  return theme('table', array(
    'header' => $header,
    'rows' => $rows,
    'empty' => $form['#empty_text'],
    'attributes' => array(
      'id' => 'inline_documentation',
    ),
  )) . drupal_render_children($form);
}

/**
 * Theme function for the form
 * 'inline_documentation_form_overview_inline_documentation_items'.
 */
function theme_inline_documentation_form_overview_inline_documentation_items($variables) {
  $form = $variables['form'];
  // Initialize rows array.
  $rows = array();
  // Iterate through the element child names.
  foreach (element_children($form['items']) as $key) {
    // Retrieve the inline documentation item element by reference.
    $child_element = &$form['items'][$key];
    // Retrieve the inline documentation item.
    $inline_documentation_item = $child_element['#inline_documentation_item'];
    // Initialize row array.
    $row = array();
    $row[] = drupal_render($child_element['view']);
    // Add the iid, parent and depth to the first cell.
    $row[0] .= drupal_render($child_element['iid']);
    $row[] = drupal_render($child_element['edit']);
    $row[] = drupal_render($child_element['delete']);
    // Add the row to the rows array.
    $rows[] = array('data' => $row);
  }

  $header = array(
    t('Name'),
    array(
      'data' => t('Operations'),
      'colspan' => 2,
    ),
  );
  return theme('table', array(
    'header' => $header,
    'rows' => $rows,
    'empty' => $form['#empty_text'],
    'attributes' => array(
      'id' => 'inline_documentation_items',
    ),
  )) . drupal_render_children($form);
}
