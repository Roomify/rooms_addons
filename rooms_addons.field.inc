<?php


/**
 * Implements hook_field_info().
 */
function rooms_addons_field_info() {
  return array(
    'rooms_addons' => array(
      'label' => t('Rooms Addons'),
      'description' => t('Bookable unit rooms addons.'),
      'settings' => array(),
      'default_widget' => 'rooms_addons_combined',
      'default_formatter' => 'rooms_addons_default',
      'property_type' => 'rooms_addons',
      'property_callbacks' => array('rooms_addons_property_info_callback'),
    ),
  );
}

/**
 * Property callback for the Entity Metadata framework.
 */
function rooms_addons_property_info_callback(&$info, $entity_type, $field, $instance, $field_type) {
  // Apply the default.
  entity_metadata_field_default_property_callback($info, $entity_type, $field, $instance, $field_type);

  // Finally add in instance specific property info.
  $name = $field['field_name'];
  $property = &$info[$entity_type]['bundles'][$instance['bundle']]['properties'][$name];
  $property['type'] = ($field['cardinality'] != 1) ? 'list<rooms_addons>' : 'rooms_addons';
  $property['property info'] = rooms_addons_data_property_info('Rooms Addons');
  $property['getter callback'] = 'entity_metadata_field_verbatim_get';
  $property['setter callback'] = 'entity_metadata_field_verbatim_set';
}

/**
 *
 */
function rooms_addons_data_property_info($name = NULL) {
  $properties = array(
    'product_id' => array(
      'label' => 'Product id',
      'type' => 'integer',
      'getter callback' => 'entity_property_verbatim_get',
      'setter callback' => 'entity_property_verbatim_set',
    ),
    'quantity' => array(
      'label' => 'Quantity',
      'type' => 'integer',
      'getter callback' => 'entity_property_verbatim_get',
      'setter callback' => 'entity_property_verbatim_set',
    ),
    'operation' => array(
      'label' => 'Operation',
      'type' => 'text',
      'getter callback' => 'entity_property_verbatim_get',
      'setter callback' => 'entity_property_verbatim_set',
    ),
    'value' => array(
      'label' => 'Value',
      'type' => 'integer',
      'getter callback' => 'entity_property_verbatim_get',
      'setter callback' => 'entity_property_verbatim_set',
    ),
    'type' => array(
      'label' => 'Type',
      'type' => 'text',
      'getter callback' => 'entity_property_verbatim_get',
      'setter callback' => 'entity_property_verbatim_set',
    ),
  );

  foreach ($properties as &$value) {
    $value += array(
      'description' => !empty($name) ? t('!label of field %name', array('!label' => $value['label'], '%name' => $name)) : '',
    );
  }

  return $properties;
}

/**
 * Implements hook_field_widget_info().
 */
function rooms_addons_field_widget_info() {
  return array(
    'rooms_addons_combined' => array(
      'label' => t('Combined text field'),
      'field types' => array('rooms_addons'),
      'settings' => array(),
    ),
  );
}

/**
 * Implements hook_field_is_empty().
 */
function rooms_addons_field_is_empty($item, $field) {
	return empty($item['product_id']);
}

/**
 * Implements hook_field_formatter_info().
 */
function rooms_addons_field_formatter_info() {
  return array(
    'rooms_addons_default' => array(
      'label' => t('Rooms Options Default'),
      'field types' => array('rooms_addons'),
    ),
    'rooms_addons_price' => array(
      'label' => t('Rooms Options Price'),
      'field types' => array('rooms_addons'),
    ),
    'rooms_addons_admin' => array(
      'label' => t('Rooms Options Administrator'),
      'field types' => array('rooms_addons'),
    ),
  );
}

/**
 * Implements hook_field_formatter_view().
 */
function rooms_addons_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
  $element = array();

  switch ($display['type']) {
    case 'rooms_addons_default':
      foreach ($items as $delta => $item) {
        $element[$delta] = array('#markup' => "{$item['quantity']} x {$item['name']}");
      }
      break;

    case 'rooms_addons_price':
      $currency_setting = commerce_currency_load(commerce_default_currency());
      $currency_symbol = $currency_setting['symbol'];

      foreach ($items as $delta => $item) {
        $price = t('@currency_symbol@amount', array(
          '@currency_symbol' => $currency_symbol,
          '@amount' => number_format($item['value'], 2, '.', ''),
        ));

        if ($item['value'] > 0) {
          $element[$delta] = array('#markup' => "{$item['quantity']} x {$item['name']} - {$price}");
        }
        else {
          $element[$delta] = array('#markup' => "{$item['quantity']} x {$item['name']}");
        }
      }
      break;

    case 'rooms_addons_admin':
      foreach ($items as $delta => $item) {
        $element[$delta] = array('#markup' => "{$item['quantity']} x {$item['name']} - {$item['operation']} {$item['value']}");
      }
      break;
  }

  return $element;
}

/**
 * Implements hook_field_validate().
 */
function rooms_addons_field_validate($entity_type, $entity, $field, $instance, $langcode, $items, &$errors) {
}

/**
 * Implements hook_field_widget_form().
 */
function rooms_addons_field_widget_form(&$form, &$form_state, $field, $instance, $langcode, $items, $delta, $element) {
  if ($instance['widget']['type'] == 'rooms_addons_combined') {
    $field_parents = $element['#field_parents'];
    $field_name = $element['#field_name'];
    $language = $element['#language'];

    $parents = array_merge($field_parents, array($field_name, $language, $delta));

    $element += array(
      '#type' => 'rooms_addons_element',
      '#default_value' => isset($items[$delta]) ? $items[$delta] : '',
    );

    $element['#attached']['css'] = array(drupal_get_path('module', 'rooms_addons') . '/css/rooms_addons_widget.css');

    return $element;
  }
}
