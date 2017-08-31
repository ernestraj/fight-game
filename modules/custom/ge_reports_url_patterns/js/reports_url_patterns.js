/**
 * @File for js customizations.
 */
(function ($) {
  Drupal.behaviors.ge_filtering = {
    attach: function (context, settings) {
      $(document).ready(function (e) {		
         //Hide all Field data  
                $(".view-field-data").each(function() {
                  if($(this).next('div').find('div.item-list').html() == '') {
                    $(this).hide();
                  } else {
                    $(".view-field-data").next('div').hide();
                  }
                });
                // Show/hide Field data 
                $(".view-field-data").click(function (e) {
                    e.preventDefault();
                    if($(this).next('div').css('display') == 'none') {
                      $(this).find('a').html("Hide Field Data");
                    } else if($(this).next('div').css('display') == 'block') {
                      $(this).find('a').html("View Field Data");
                    }
                    $(this).next('div').slideToggle('down');
                });
            });
		}
    };   
}(jQuery));
