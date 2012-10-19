<div class="manage_collection">
  <fieldset>
    <legend>Choose a management option</legend>
    <ul>
      <li>
	<a href="#collection_add_dataset">Add Dataset By Package</a>
      </li>
      <li>
	<a href="#collection_manage">Manage This Collection</a>
      </li>
      <li>
	<a href="#object_details">Object Details</a>
      </li>
    </ul>
  </fieldset>
</div>
<div style="display:none">
  <div id="collection_add_dataset" class="esdora-fancybox">
    <?php print drupal_render($add); ?>
    <p/>
    <b/>
    <a onclick="$.fancybox.close();" href="javascript:;">
      <center>Close</center>
    </a>
  </div>
  <div id="collection_manage" class="esdora-fancybox">
    <?php print drupal_render($manage); ?>
    <p/>
    <br/>
    <a onclick="$.fancybox.close();" href="javascript:;">
      <center>Close</center>
    </a>
  </div>
  <div id="object_details" class="esdora-fancybox">
    <?php print drupal_render($details); ?>
    <p/>
    <br/>
    <a onclick="$.fancybox.close();" href="javascript:;">
      <center>Close</center>
    </a>
  </div>
</div>