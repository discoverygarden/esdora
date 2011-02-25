<?php

// $Id$

class Esdora {

  function __construct($pid = '') {
    module_load_include('inc', 'fedora_repository', 'api/fedora_item');
    if (!empty($pid)) {
      $this->pid = $pid;
      $this->item = new Fedora_Item($pid);
    }
  }

  function htmlSnippet($fieldName, $value) {
    $html = '</td></tr>';
    $html .='<tr><td valign="top" align="right" class="mods_field"><strong>' . $fieldName . '</strong></td><td>';
    if (!empty($value)) {
      foreach ($value as $va) {
        $html .= $va . '<br />';
      }
      return $html;
    }
    else {
      return '';
    }
  }

  function getFdgcAsHtml($xmlString) {
    $fdgcXml = new SimpleXMLElement($xmlString);
    $title = $fdgcXml->xpath('//title');
    $links = $fdgcXml->xpath('//onlink');
    $resdesc = $fdgcXml->xpath('//resdesc');
    $contactPerson = $fdgcXml->xpath('//cntper');
    $html = '';

    $html .= '<table align="left"><tr><td valign="top" align="right" class="fedora_mods_field" width="15%">';
    $html .='<strong>Title</strong></td><td>';
    foreach ($title as $t) {
      $html .= $t . '<br />';
    }
    $html .= '</td></tr><tr><td valign="top" align="right"><strong>Links</strong></td><td>';
    foreach ($links as $t) {
      $html .= $t . '<br />';
    }
    $html .= $this->htmlSnippet("Contact", $contactPerson);
    $html .= $this->htmlSnippet("Description", $resdesc);
    $html .='</td></tr></table></div></div><br />';
    return $html;
  }

  //finds datastreams that have a certain mimetype
  //excludes ones that contain any strings in the array $exclude
  function getListOfStreams($mimeTypeToRetrieve, $exclude=null) {
    // $list_of_streams = array();
    global $base_url;
    $html = '';
    $this->datastreams_list = $this->item->get_datastreams_list_as_SimpleXML();
    if ($this->datastreams_list != FALSE) {
      foreach ($this->datastreams_list->datastreamDef as $ds) {
        if ($mimeTypeToRetrieve == $ds->MIMEType || $mimeTypeToRetrieve == '*') {
          // $list_of_streams[] = $ds;
          $include = TRUE;
          if (isset($exclude)) {
            foreach ($exclude as $value) {
              if (stristr($ds->ID, $value)) {
                $include = false;
              }
            }
          }
          if ($include) {
            $label = $ds->label;
            if (empty($label)) {
              $label = $ds->ID;
            }
            $html .= '<a href = "' . $base_url . '/fedora/repository/' . $this->pid . '/' . $ds->ID . '/' . drupal_urlencode($label) . '">' . $ds->label . '</a><br />';
          }
        }
      }
    }
    //return $list_of_streams;
    return $html;
  }

  //finds datastreams that have a certain mimetype
  //excludes ones that contain any strings in the array $exclude
  //returns an array of streams we are interested in
  function getListOfStreamsRaw($mimeTypeToRetrieve, $exclude=null) {
    $list_of_streams = array();
    global $base_url;
    $html = '';

    if ($this->datastreams_list != FALSE) {
      foreach ($this->datastreams_list->datastreamDef as $ds) {
        if ($mimeTypeToRetrieve == $ds->MIMEType || $mimeTypeToRetrieve == '*') {
          // $list_of_streams[] = $ds;
          $include = TRUE;
          if (isset($exclude)) {
            foreach ($exclude as $value) {
              if (stristr($ds->ID, $value)) {
                $include = false;
              }
            }
          }
          if ($include) {
            $label = $ds->label;
            if (empty($label)) {
              $label = $ds->ID;
            }
            $list_of_streams[] = $ds->ID . '/' . $label;
          }
        }
      }
    }

    return $list_of_streams;
  }

  public function showFieldSets() {
    global $base_url;
    $objectHelper = new ObjectHelper();
    global $base_url;
    global $user;
    $fdgcText = $this->item->get_datastream_dissemination("fdgc.xml");
    $fdgcAsHtml = $this->getFdgcAsHtml($fdgcText);

    $tabset = array();

    $tabset['my_tabset'] = array(
      '#type' => 'tabset',
    );

    global $user;
    $streams = $this->getListOfStreams('*');
    $textStreams = $this->getListOfStreamsRaw('text/plain');
    $pdfStreams = $this->getListOfStreamsRaw('application/octet-stream');

    $content = '';
    $fieldSet = null;
    foreach ($pdfStreams as $pdfStream) {
      if (stripos($pdfStream, 'readme')) {
        $pdfDsid = substr($pdfStream, strpos($pdfStream, '/') + 1);
        $pdfUrl = '<div class="text"><iframe src="http://docs.google.com/viewer?url=$base_url/fedora/repository/' .
            $this->pid . '/' . $pdfDsid . '/preview.pdf' . '&embedded="TRUE" style="width:600px; height:500px;" frameborder="0"></iframe>S</div>';
        $fieldSet = array(
          '#title' => $pdfDsid,
          '#attributes' => array(),
          '#collapsible' => TRUE,
          '#collapsed' => TRUE,
          '#value' => $pdfUrl,
        );
        $content .= theme('fieldset', $fieldSet);
      }
    }
    foreach ($textStreams as $txtStream) {
      $txtDsid = substr($txtStream, strpos($txtStream, '/') + 1);
      $txtValue = trim($this->item->get_datastream_dissemination($txtDsid));
      if (!empty($txtValue)) {
        $fieldSet = array(
          '#title' => $txtDsid,
          '#attributes' => array(),
          '#collapsible' => TRUE,
          '#collapsed' => TRUE,
          '#value' => '<div class="text">' . $txtValue . '</div>'
        );
        $content .= theme('fieldset', $fieldSet);
      }
    }
    $tabset['my_tabset'][0] = array(
      '#type' => 'tabpage',
      '#title' => t('Metadata'),
      '#content' => $fdgcAsHtml,
    );

    //$content=$objectHelper->getQDC($this->pid);

    $tabset['my_tabset'][1] = array(
      // #type and #title are the minimum requirements.
      '#type' => 'tabpage',
      '#title' => t('Description'),
      // This will be the content of the tab.
      '#content' => $content,
    );
    $tabset['my_tabset'][2] = array(
      // #type and #title are the minimum requirements.
      '#type' => 'tabpage',
      '#title' => t('Items'),
      // This will be the content of the tab.
      '#content' => $streams,
    );


    return tabs_render($tabset);
  }

}