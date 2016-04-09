<?php
/**
 * Available variables:
 * - $hash
 * - $name
 * - $description
 * - $items
 */
?>

<div class="inline_documentation inline_documentation-<?php print $hash;?>">
  <h2><?php print $name;?></h2>
  <p><?php print $description;?></p>
  <?php if (!empty($items)): ?>
  <div id = "inline_documentation-items-<?php print $hash;?>" class = 'inline_documentation-items'>
    <?php foreach ($items as $item): ?>
      <?php print $item;?>
    <?php endforeach;?>
  </div>
  <?php endif;?>
</div>