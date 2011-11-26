<?php

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

class EsdoraCollection extends CollectionClass {

    function __construct($pid = '') {
        module_load_include('inc', 'fedora_repository', 'api/fedora_item');
        module_load_include('inc', 'fedora_repository', 'CollectionClass');
        if (!empty($pid)) {
            $this->pid = $pid;
            $this->item = new Fedora_Item($pid);
        }
    }

    function showCollection() {
        global $base_url;
        global $user;
        $path = drupal_get_path('module', 'esdora_islandora');

        $dc = $this->item->get_datastream_dissemination('DC');
        //return 'test';
        $xml = new SimpleXMLElement($dc);
        $returnValue = '';
        $collectionName = '';
        foreach ($xml->children('http://purl.org/dc/elements/1.1/') as $key => $entry) {
            if ($key == 'description') {//get the dc description and or title
                $returnValue .= ' ' . (string) $entry;
            }
            if ($key == 'title') {//get the dc description and or title
                $collectionName = (string) $entry;
            }
        }

        $item = new Fedora_Item($this->pid);
        $query = NULL;
        if ($item->exists() && array_key_exists('QUERY', $item->datastreams)) {
            $query = $item->get_datastream_dissemination('QUERY');
        }
        if ($query == NULL) {
            //because of the multiple models in the collection object, 
            //need a slightly different query form Islandora, for all ESDORA colleciton object
            //that does not have the QUERY datastream
            $query = 'select $object $title $content
                from <#ri>
                where 
                ($object <dc:title> $title
                and $object <fedora-model:hasModel> $content
                and ($object <fedora-rels-ext:isMemberOfCollection> <info:fedora/' . $this->pid . '>
                    or $object <fedora-rels-ext:isMemberOf> <info:fedora/' . $this->pid . '>)
                and $object <fedora-model:state> <info:fedora/fedora-system:def/model#Active>)
                minus $content <mulgara:is> <info:fedora/fedora-system:FedoraObject-3.0>
                minus $content <mulgara:is> <info:fedora/islandora:collectionCModel>
                order by $title';
        }
        $collectionList = '';
        $content = $this->getRelatedItems($this->pid, $query);

        $xslContent = $this->getXslContent($this->pid, $path);

        if (isset($content) && $content != FALSE) {
            $input = new DomDocument();
            $input->loadXML(trim($content));
            $results = $input->getElementsByTagName('result');
            if ($results->length > 0) {
                try {
                    $proc = new XsltProcessor();
                    $proc->setParameter('', 'collectionPid', $this->pid);
                    $proc->setParameter('', 'collectionTitle', $collectionName);
                    $proc->setParameter('', 'baseUrl', $base_url);
                    $proc->setParameter('', 'path', $base_url . '/' . $path);
                    $proc->setParameter('', 'hitPage', $pageNumber);
                    $proc->registerPHPFunctions();
                    $xsl = new DomDocument();
                    $xsl->loadXML($xslContent);
                    $xsl = $proc->importStylesheet($xsl);
                    $newdom = $proc->transformToDoc($input);

                    $objectList = $newdom->saveXML(); //is the xml transformed to html as defined in the xslt associated with the collection object

                    if (!$objectList) {
                        throw new Exception("Invalid XML.");
                    }
                    $collectionList = $objectList;
                } catch (Exception $e) {
                    drupal_set_message(t('!e', array('!e' => $e->getMessage())), 'error');
                    return '';
                }
            }
        } else {
            drupal_set_message(t("No Objects in this collection or bad query."));
        }

        return $returnValue . $collectionList;
    }

    function getXslContent($pid, $path, $canUseDefault = TRUE) {
        module_load_include('inc', 'fedora_repository', 'CollectionClass');
        $collectionClass = new CollectionClass();
        $xslContent = $collectionClass->getCollectionViewStream($pid);
        if (!$xslContent && $canUseDefault) { //no xslt so we will use the default sent with the module
            $xslContent = file_get_contents($path . '/xsl/sparql_to_html.xsl');
        }
        return $xslContent;
    }

 
    function getContentForDisplay($esdora_collection_model, $hasManagePermission, $page_number) {
//note: copied from CollectionClass->showFieldSets($page_number)
//so ESDORA will no longer need the Islandora Conent Model and we can delete some tab views
//this is for Collection Object only     
//TODO: the tabs jumps and need to have a way to rem the position of the selected tab
        module_load_include('inc', 'fedora_repository', 'ContentModel');
        module_load_include('inc', 'fedora_repository', 'plugins/FedoraObjectDetailedContent');
        module_load_include('inc', 'fedora_repository', 'api/fedora_item');
        module_load_include('inc', 'fedora_repository', 'CollectionManagement');
        module_load_include('inc', 'fedora_repository', 'BatchIngest');

        $esdora_collection_display = $esdora_collection_model->displayExtraFieldset($this->pid, $page_number);//calls showCollection
        if (!$hasManagePermission) {
            return $esdora_collection_display;
        }
        //else we are putting a tabbed display with an overall manage tab
        //Nov 20, changed to fancyox popups -jerry

        $add_to_collection = $this->getIngestInterface();
//        $show_batch_tab = TRUE;
        $policy = CollectionPolicy::loadFromCollection($this->pid, TRUE);
        $content_models = $policy->getContentModels();
//        if (count($content_models) == 1 && $content_models[0]->pid == "islandora:collectionCModel") {
//            $show_batch_tab = FALSE;
//        }
/*        
        $tabset = array(
           '#type' => 'tabset',
            );
        $tabset['add_tab'] = array(
            '#type' => 'tabpage',
            '#title' => t('Add Dataset'),
            '#selected' => TRUE,
            // This will be the content of the tab.
            '#content' => $add_to_collection,
        );
        if (user_access('manage collections')) {
            $tabset['add_collection_tab'] = array(
                // #type and #title are the minimum requirements.
                '#type' => 'tabpage',
                '#title' => t('Manage Collection'),
                // This will be the content of the tab.
                '#content' => drupal_get_form('collection_management_form', $this->pid, $content_models),
                '#selected' => FALSE,
            );
        }
        if ($show_batch_tab && user_access('create batch process')) {
            $tabset['batch_ingest_tab'] = array(
                // #type and #title are the minimum requirements.
                '#type' => 'tabpage',
                '#title' => t('Batch Ingest'),
                // This will be the content of the tab.
                '#content' => drupal_get_form('batch_creation_form', $this->pid, $content_models),
            );
        }
  */      

//experiment with lightbox
//TODO: refactoring and extract functions
$add_to_collection_view = array(
    '#type' => 'fieldset',
    '#title' => t('Ingest Dataset'),
    '#value' => $add_to_collection,
);

$manage_collection_view = array(
    '#type' => 'fieldset',
    '#title' => t('Manage Collection'),
    '#value' => drupal_get_form('collection_management_form', $this->pid, $content_models),
    );

$obj = new FedoraObjectDetailedContent($this->pid);
$object_details = $obj->showFieldSets();
        
drupal_add_css("sites/all/libraries/fancybox/jquery.fancybox-1.3.4.css"); 
drupal_add_js("sites/all/libraries/fancybox/jquery.fancybox-1.3.4.js");  
drupal_add_js (        
    '$(document).ready(function(){
        $("div.manage_collection a").fancybox({
            \'overlayShow\': true,
            \'titlePosition\' : \'inside\',
            \'transitionIn\' : \'none\',
            \'transitionOut\' : \'none\',
        });
     }); ',
    'inline');

$url_add_dataset = "#collection_add_dataset";
$url_manage_collection = "#collection_manage";
$url_object_details = "#object_details";

$fancybox_width='800px;';
$fancybox_height='650px;';

$my_manage_view = '<div class="manage_collection">';
$my_manage_view .= '<fieldset><legend>Choose a management option</legend>';
$my_manage_view .= '<ul>';
$my_manage_view .= '<li><a href=' . $url_add_dataset . '>Add Dataset By Package</a></li>';
$my_manage_view .= '<li><a href=' . $url_manage_collection . '>Manage This Collection</a></li>';
$my_manage_view .= '<li><a href=' . $url_object_details . '>Object Details</a></li></ul></fieldset></div>';

$my_manage_view .= '<div style="display: none;">                
        
        <div id="collection_add_dataset" style="width:' .$fancybox_width .' height:'.$fancybox_height .' overflow:auto;">' 
                . drupal_render($add_to_collection_view) .   
        '<p/><br/><a onclick="$.fancybox.close();" href="javascript:;"><center>Close</center></a></div>
            
        <div id="collection_manage" style="width:' .$fancybox_width .' height:'.$fancybox_height .' overflow:auto;">' 
                . drupal_render($manage_collection_view) .   
        '<p/><br/><a onclick="$.fancybox.close();" href="javascript:;"><center>Close</center></a></div>

        <div id="object_details" style="width:' .$fancybox_width .' height:'.$fancybox_height .' overflow:auto;">' 
                . drupal_render($object_details) .   
        '<p/><br/><a onclick="$.fancybox.close();" href="javascript:;"><center>Close</center></a></div>
';
        
        $esdora_tabs = array(
            '#type' => 'tabset',
        );
        $esdora_tabs['collection_view'] = array(
            '#type' =>'tabpage',
            '#title'=>'View',
            '#content'=>$esdora_collection_display,
        );
        $esdora_tabs['manage_view'] = array(
            '#type' => 'tabpage',
            '#title' => t('Manage Collection'),
            '#content' => $my_manage_view,
        );
        
        return tabs_render($esdora_tabs);
    }
}

?>
