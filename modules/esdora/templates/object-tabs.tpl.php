<div id='dataset-ajax-tabs'>
  <button id='items'>Items</button>
  <button id='metadata'>Metadata</button>
  <button id='description'>Description</button>
  <button id='history'>History</button>
  <?php if ($can_manage): ?>
    <button id='manage'>Manage</button>
  <?php endif;?>
  <?php if ($has_viewers): ?>
    <button id='viewers'>Viewers</button>
  <?php endif;?>
</div>
<div id='div1'>
  <?php print $active_tab; ?>
</div>
<input type='hidden' id='pid' value='<?php print $pid; ?>'/>
