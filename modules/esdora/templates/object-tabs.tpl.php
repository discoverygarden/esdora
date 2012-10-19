<div id='dataset-ajax-tabs'>
  <button id='Items'>Items</button>
  <button id='Metadata'>Metadata</button>
  <button id='Description'>Description</button>
  <button id='History'>History</button>
  <?php if ($can_manage): ?>
    <button id='Manage'>Manage</button>
  <?php endif;?>
  <?php if ($has_viewers): ?>
    <button id='Viewers'>Viewers</button>
  <?php endif;?>
</div>
<div id='div1'>
  <?php print $active_tab; ?>
</div>
<input type='hidden' id='pid' value='<?php print $pid; ?>'/>
