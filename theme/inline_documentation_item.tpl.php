
<div id="inline_documentation-item-wrapper-<?php print $inline_documentation_item->hash;?>" class="inline_documentation-item-wrapper">
  <div id="inline_documentation-item-name-<?php print $inline_documentation_item->hash;?>" class="inline_documentation-item-name">
    <?php print $inline_documentation_item->name;?>
    <p><?php print $inline_documentation_item->description;?></p>
    <?php print render($content);?>
    <?php //dpm(get_defined_vars());?>
  </div>
</div>