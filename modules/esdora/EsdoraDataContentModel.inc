<?php

/**
 * @file 
 * 
 */
/**
 * @file 
 * Content Model Class
 */
module_load_include('inc', 'fedora_repository', 'ContentModel');

/**
 * Conent Model ??
 */
class EsdoraDataContentModel extends ContentModel {

    /**
     * Constructs a ContentModel object from the PID of the model in Fedora.
     * If DSID is specified it will use that datastream as the model, otherwise it will
     * use the default (usually ISLANDORACM).  PID_NAMESPACE and name can also be initialized
     * from the collection policy.
     * Returns FALSE on failure.
     *
     * NOTE: $name will be overwritten with the content model name found in the datastream
     * when the model is first validated.\
     *
     * @param string $pid
     * @param string $dsid
     * @param string $pid_namespace
     * @param string $name
     * @return EsdoraDataContentModel $cm
     */
    public static function loadFromModel($pid, $dsid = NULL, $pid_namespace = NULL, $name = NULL) {
        $ret = FALSE;
        module_load_include('inc', 'fedora_repository', 'api/fedora_item');
        try {
            if (self::valid_pid($pid)) {
                $fedoraItem = new Fedora_Item($pid);
                if (!$name) {
                    $name = $fedoraItem->objectProfile->objLabel;
                }
                $dsid = ($dsid != NULL && self::valid_dsid($dsid)) ? $dsid : ContentModel::getDefaultDSID();
                $ds = $fedoraItem->get_datastream_dissemination($dsid);
                if (!empty($ds)) {
                    $ret = new EsdoraDataContentModel($ds, $pid, $dsid, $pid_namespace, $name);
                }
            }
        } catch (SOAPException $e) {
            $ret = FALSE;
        }
        return $ret;
    }

    /**
     * Constructor
     * NOTE: Use the static constructor methods whenever possible.
     *
     * @param string $xmlStr
     * @param string $pid
     * @param string $dsid
     * @param string $pid_namespace
     * @param string $name
     * @return XMLDatastream $cm
     */
    public function __construct($xmlStr, $pid = NULL, $dsid = NULL, $pid_namespace = NULL, $name = NULL) {
        parent::__construct($xmlStr, $pid, $dsid);
        $this->pid_namespace = $pid_namespace;
        $this->name = ($name == NULL) ? 'Islandora Content Model' : $name;
        $this->xpath = new DOMXPath($this->xml);
        $this->xpath->registerNamespace('cm', 'http://www.islandora.ca');
    }

    
    
    public function ingestZippedPackage($collection_pid, $file, $dataset_name) {

        $handlers = $this->xml->getElementsbyTagName("package_ingest_handler");
        if (!isset($handlers)) {
            throw new Exception("Content model $cmodel_pid does not define a 'package_ingest_handler', which is required for this action");
        }

        $handler = $handlers->item(0); //Q - multiple handlers is possible here, but not needed for now
        $module = $handler->getAttribute('module');
        $className = $handler->getAttribute('class');
        $methodName = $handler->getAttribute('method');
        $moduleFile = $handler->getAttribute('file');
        $path = drupal_get_path('module', !empty($module) ? $module : 'esdora_islandora') . '/' . $moduleFile;
        if (!file_exists($path)) {
            drupal_set_message('Ingest handler file \'' . $path . '\' does not exist.', 'error');
            return;
        }
        @require_once($path);
        try {
            $class = new $className;
            $pid=$class->$methodName($collection_pid, $file, $this->pid, $dataset_name); //$this->pid is the cmodel pid, function returns newly created pid
            return $pid;
            //example call:
            //    $esdora_ingest = new EsdoraIngest();
            //    $esdora_ingest->handleZipIngestForm($collection_pid, $file);                
        } catch (Exception $e) {
            Drupal_set_message('Error calling Ingest handler method \'' . $className . '->' . $methodName . $e->getMessage());
            return NULL;
        }
    }
}