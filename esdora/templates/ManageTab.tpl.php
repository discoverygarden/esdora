<div class="manage_dataset">
  <fieldset>
    <legend>Choose a management option</legend>
    <ul>
      <li><a href="#dataset_add_component">Add a component</a></li>
      <li><a href="#dataset_edit_fgdc">Edit Metadata (FGDC)</a></li>
      <li><a href="#dataset_add_lineage">Data Lineage</a></li>
      <li><a href="#dataset_object_details">Object Details</a></li>
      <li><a href="#add_to_workflow">Add To Workflow</a></li>
    </ul>
  </fieldset>
</div>
<div style="display: none;">
  <div id="dataset_add_component" class="esdora-fancy-box-item">
    <?php print drupal_render($manage_view_add_component); ?>
    <br/>
    <a onclick="$.fancybox.close();" href="javascript:;">
      <center>Close</center>
    </a>
  </div>
  <div id="dataset_edit_fgdc" class="esdora-fancy-box-item" >
    <?php print drupal_render($manage_view_edit_fgdc); ?>
    <br/>
    <a onclick="$.fancybox.close();" href="javascript:;">
      <center>Close</center>
    </a>
  </div>
  <div id="dataset_edit_dc"  class="esdora-fancy-box-item">
    <?php print drupal_render($manage_view_edit_dc); ?>
    <br/>
    <a onclick="$.fancybox.close();" href="javascript:;">
      <center>Close</center>
    </a>
  </div>
  <div id="dataset_add_lineage" class="esdora-fancy-box-item">
    <?php print drupal_render($manage_view_lineage); ?>
    <br/>
    <a onclick="$.fancybox.close();" href="javascript:;">
      <center>Close</center>
    </a>
  </div>
  <div id="dataset_object_details" class="esdora-fancy-box-item">
    <?php  print drupal_render($object_details); ?>
    <br/>
    <a onclick="$.fancybox.close();" href="javascript:;">
      <center>Close</center>
    </a>
  </div>
  <div id="add_to_workflow" class="esdora-fancy-box-item">
    <?php  print drupal_render($add_to_workflow); ?>
    <br/>
    <a onclick="$.fancybox.close();" href="javascript:;">
      <center>Close</center>
    </a>
  </div>
</div>
