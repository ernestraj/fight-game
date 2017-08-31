<?php
/**
* @file
* Main page template.
*/
?>

<div id="branding1" class="clearfix">

<div class="workbech_menu">
  <?php

  $block = module_invoke('menu', 'block_view', 'menu-workbench-link-menu');
  
    print render($block['content']);
  ?>
 </div>
 
<div class="user-info-block">
  <?php
    $block = module_invoke('ge_dashboard', 'block_view', 'real_name_block');
    print render($block['content']);
  ?>
 </div>

 <div class="search-new-block">
  <?php
    $block = module_invoke('views', 'block_view', '-exp-searchnew-page');
    $output = render($block['content']);
    print $output;
  ?>
 </div>
  <div class="country-switch-block">
  <?php
    $block = module_invoke('country', 'block_view', 'country_selection');
    print render($block['content']);
  ?>
 </div>
</div>

<div id="branding" class="clearfix">



	<?php print $breadcrumb; ?>

	<?php print render($title_prefix); ?>

	<?php if ($title): ?>
		<h1 class="page-title"><?php print $title; ?></h1>
	<?php endif; ?>

	<?php print render($title_suffix); ?>
	<?php if (!$is_front): ?>
	<?php print render($primary_local_tasks); ?>
	<?php endif;?>
</div></div>

<div id="page">
	<?php if ($secondary_local_tasks): ?>
		<div class="tabs-secondary clearfix"><ul class="tabs secondary"><?php print render($secondary_local_tasks); ?></ul></div>
	<?php endif; ?>

	<div id="content" class="clearfix">
		<div class="element-invisible"><a id="main-content"></a></div>

	<?php if ($messages): ?>
		<div id="console" class="clearfix"><?php print $messages; ?></div>
	<?php endif; ?>

	<?php if ($page['help']): ?>
		<div id="help">
			<?php print render($page['help']); ?>
		</div>
	<?php endif; ?>

	<?php if (isset($page['content_before'])): ?>
		<div id="content-before">
			<?php print render($page['content_before']); ?>
		</div>
	<?php endif; ?>

	<?php if ($action_links): ?><ul class="action-links"><?php print render($action_links); ?></ul><?php endif; ?>

	<?php print render($page['content']['system_main']); ?>

	<?php if (isset($page['content_after'])): ?>
		<div id="content-after">
			<?php print render($page['content_after']); ?>
		</div>
	<?php endif; ?>

	</div>

	<div id="footer">
		<?php print $feed_icons; ?>
	</div>

</div>
