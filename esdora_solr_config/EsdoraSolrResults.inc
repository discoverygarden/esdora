<?php

/*
 * contains methods to search solr and display results.  depends on Apache_Solr_Php client.
 */


/**
 * Extention of IslandoraSolrResults for templating purposes.
 * This overrides the displayResults function to provide an alternate display type.
 */

class EsdoraSolrResults
extends IslandoraSolrResults {

  static $facetSeparator = '~';//used to separate facets in url
  static $slashReplacement = '~slsh~';// a pattern used to replace / in url's the slash breaks drupal clean url's
  public static $SEARCH_CLASS_ADVANCED_SEARCH_NUMBER_FIELDS = 5;
  function IslandoraSolrResults() {
    module_load_include('php', 'islandora_solr_search', 'Solr/Service');
  }

  /**
   * Outputs results in unified tabular format.
   *
   * This function collects all field names from all results in the 
   * resultset and constructs a single table with said field names in 
   * a unified header row, rather than outputting each result as a 
   * miniature horizontal table.
   *
   * @param $results
   *   A solr resultset object.
   *
   * @return
   *   html output for the resultset. Note: we currently create this 
   *   output manually, should refactor to use drupal forms api.
   */
  function displayResults($results){

    global $base_url;
    if(empty($results)) {
      return "no results";
    }

    $moduleRoot=drupal_get_path('module','islandora_solr_search');
    drupal_add_css($moduleRoot.'/css/uofm.css');
    drupal_add_css($moduleRoot.'/css/jquery.lightbox-0.5.css');
    drupal_add_js($moduleRoot.'/js/jquery.lightbox-0.5.modified.js');
    drupal_add_js($moduleRoot.'/js/uofm.js');

    drupal_add_js("$(document).ready(function () {  $('.results a.zoomIcon').lightBox( { txtImage: 'Result', imageLoading: '$base_url/$moduleRoot/images/lightbox-icon-loading.gif',  imageBtnClose: '$base_url/$moduleRoot/images/lightbox-btn-close.gif',  imageBtnPrev: '$base_url/$moduleRoot/images/lightbox-btn-prev.gif', imageBtnNext: '$base_url/$moduleRoot/images/lightbox-btn-next.gif'  });  });",'inline');
    $output = '';
    $output.='<div class="results"/><table border="0" cellpadding="10" cellspacing="0" width="100%">';
		$arg = join('/',arg());
    foreach($results->response->docs as $doc) {


      $pid=$doc->PID;//htmlspecialchars(preg_replace('/\//',':',$doc->id),ENT_QUOTES,'utf-8');
      //Title,Subject,Format,Author,Type,Description,PID
      $title = $doc->Title;
      if(is_array($doc->Title)){
        $title = (string)$title[0];
      }
      $output .= '<tr><td><div class="thumbnail">';
			$output.='<a href="'.$base_url.'/fedora/repository/'.$pid.'" title="'.$title.'"><img src="'.$base_url.'/fedora/'.$pid.'/TN"/></a></div></td>';
      $output .= '<td><div class="metadata"><a href="'.$base_url.'/fedora/repository/'.$pid.'">'.$title.'</a><ul>';
			if (!empty($doc->Creator)){
		 	 $output.='<li><b>Author:</b>&nbsp;';
       if(is_array($doc->Creator)){
       foreach ($doc->Creator as $author) {
         $output .= "$author, ";
        }
       } else {
         $output .=$doc->Creator;
       }
        $output .= '</li>';
      }
      if (!empty($doc->Keywords)){
					$output.='<li><b>Keywords:</b>&nbsp;';
          if(is_array($doc->Keywords)){
        foreach ($doc->Keywords as $subject) {
         $output .= "$subject, ";
        }
          }else{
            $output .= $doc->Subject;
          }
        $output .= '</li>';
      }

      if (!empty($doc->Summary)){
		 	 $output.='<li><b>Format:</b>&nbsp;';
       if(is_array($doc->Summary)){
       foreach ($doc->Summary as $summary) {
         $output .= "$summary, ";
        }
       } else {
         $output .=$doc->Summary;
       }
        $output .= '</li>';
      }

      $output.='</ul></div></td></tr>';

     }
    $output.='</table></div>';

    return $output;
  }

}

