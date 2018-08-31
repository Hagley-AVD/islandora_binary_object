<?php
/**
 * @file
 * Handles the display of binary objects.
 */

/**
 * Implements hook_preprocess_theme().
 */
function template_preprocess_islandora_binary_object(array &$variables) {
  module_load_include('inc', 'islandora', 'includes/metadata');
  drupal_add_js('misc/form.js');
  drupal_add_js('misc/collapse.js');

  $islandora_object = $variables['islandora_object'];

  if (isset($islandora_object['OBJ']) && islandora_datastream_access(ISLANDORA_VIEW_OBJECTS, $islandora_object['OBJ'])) {
    $mime_detect = new MimeDetect();
    $extension = $mime_detect->getExtension($islandora_object['OBJ']->mimetype);

    $variables['islandora_binary_object_info'] = t('This is a downloadable object of filetype @extension and size @size.', array(
      '@extension' => $extension,
      '@size' => format_size($islandora_object['OBJ']->size),
    ));

    $display['download'] = array(
      '#type' => 'item',
      '#title' => t('Download'),
      '#markup' => l(t('@label', array('@label' => $islandora_object['OBJ']->label)), "islandora/object/$islandora_object->id/datastream/OBJ/download"),
    );
    $variables['islandora_binary_object_download'] = drupal_render($display['download']);
  }

  // Thumbnail.
  if (isset($islandora_object['TN']) && islandora_datastream_access(ISLANDORA_VIEW_OBJECTS, $islandora_object['TN'])) {
    $thumbnail_size_url = url("islandora/object/{$islandora_object->id}/datastream/TN/view");
    $params = array(
      'title' => $islandora_object->label,
      'path' => $thumbnail_size_url,
    );
    $variables['islandora_thumbnail_img'] = theme('image', $params);
  }
  $variables['parent_collections'] = islandora_get_parents_from_rels_ext($islandora_object);
  $variables['metadata'] = islandora_retrieve_metadata_markup($islandora_object);
  $variables['description'] = islandora_retrieve_description_markup($islandora_object);

//extracted binaries
  if (isset($islandora_object['BINARY']) && islandora_datastream_access(ISLANDORA_VIEW_OBJECTS, $islandora_object['BINARY'])) {
    $mime_detect = new MimeDetect();
    $extension = $mime_detect->getExtension($islandora_object['BINARY']->mimetype);

    $variables['islandora_binary_datastream_info'] = t('This is a downloadable object of filetype @extension and size @size.', array(
      '@extension' => $extension,
      '@size' => format_size($islandora_object['BINARY']->size),
    ));

    $display['download'] = array(
      '#type' => 'item',
      '#title' => t('Download'),
      '#markup' => l(t('@label', array('@label' => $islandora_object['BINARY']->label)), "islandora/object/$islandora_object->id/datastream/BINARY/download"),
    );
    $variables['islandora_binary_datastream_download'] = drupal_render($display['download']);
  }

//sample video
  $viewer_dsid = 'MP4';
  $mime = 'video/mp4';

  $video_params = array(
    'pid' => $islandora_object->id,
  );
  // Video player.
  if (isset($islandora_object[$viewer_dsid]) && islandora_datastream_access(ISLANDORA_VIEW_OBJECTS, $islandora_object[$viewer_dsid])) {
    $video_url = url("islandora/object/{$islandora_object->id}/datastream/$viewer_dsid/view");
    $video_params += array(
      'mime' => $mime,
      'url' => $video_url,
    );
  }
$resourceURL = url("http://ingest.hagley.dgicloud.com/$video_url");
$ch = curl_init($resourceURL);
curl_setopt($ch, CURLOPT_NOBODY, true);
curl_exec($ch);
$statusCode = curl_getinfo($ch, CURLINFO_HTTP_CODE);
curl_close($ch);
if ($statusCode != '200'){
$viewer = islandora_get_viewer($video_params, 'islandora_video_viewers', $islandora_object);
  $variables['islandora_content'] = '';
  if ($viewer) {
    $variables['islandora_content'] = $viewer;
  }
  }else {
    $variables['islandora_content'] = NULL;
  }

  return array('' => $viewer);
}
