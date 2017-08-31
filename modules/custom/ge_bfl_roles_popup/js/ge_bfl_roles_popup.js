(function($, Drupal) {
  Drupal.behaviors.bflpopup = {      
    attach: function(context, settings) {
      $(document).ready(function(e) {
        //jQuery selector for finding all checkboxes on modal page
        var checkboxes = '#modal-content .view-id-tax.view-display-id-entityreference_view_widget_1 .form-checkbox';
        $(checkboxes).live('click', function() {
          //Check if checkbox has been checked
          if(this.checked) {
            var myvalue=$(this).val();
            //Make the checked option selected in chosen widget 
            $('#edit-selected-bfl option[value="'+myvalue+'"]').attr('selected', 'selected');
            $('#edit-selected-bfl').chosen().change();
            $('#edit-selected-bfl').trigger('liszt:updated');
          }
          else {
            var myvalue=$(this).val();
            //Make the checked option selected in chosen widget 
            $('#edit-selected-bfl option[value="'+myvalue+'"]').attr('selected', '');
            $('#edit-selected-bfl').chosen().change();
            $('#edit-selected-bfl').trigger('liszt:updated');
          }
        });
		
        var data = Drupal.settings['ge_bfl_roles_popup'];
        if (typeof data !== "undefined") {
          var field_html = window.parent.document.getElementById(data.field_name);
          var field_html_trimmed = rtrim(field_html.value, ',');
          var options = field_html_trimmed.split(",");
          if (options[0].length > 1) {
            if (data.is_bfl) {
              $.each(options, function (index, item) {
                if (item.indexOf("(id:") > 1) {
                  var val = item.split("(id:")[1].replace(")", "");
                  var option_text = item.split("(id:")[0];
                  $('.form-item.form-item-selected-bfl > select > option[value="' + val + '"]').attr("selected", "selected");
		}
              });
            }
            else {
              $.each(options, function (index, item) {
                $(".form-item select option").filter(function() {
                  return $(this).text() === item.trim();
                }).attr("selected", true);
              });
            }
            $(".form-item select").trigger("chosen:updated");
          }
        }
      });
    }
  }
  $("#edit-selected-bfl").chosen().change(function(event) {
    var selNodes = node.tree.getSelectedNodes();
    var aux = 0;
    //convert to title/key array
    var selKeys = $.map(selNodes, function(node){
      node.select(1);	
    });
    var options=$(this).val();
    var count = options.length - 1;
    if(count>0) {
      $.each(options, function (index, item) {	   
        $("#tree").dynatree("getTree").getNodeByKey(item).select();
        var node = $("#tree").dynatree("getTree").getNodeByKey(item);
        node.expand();
      });
    }		
  });
  function rtrim(str, chr) {
      var rgxtrim = (!chr) ? new RegExp('\\s+$') : new RegExp(chr+'+$');
      return str.replace(rgxtrim, '');
  }
    // Our function name is prototyped as part of the Drupal.ajax namespace, adding to the commands:
  Drupal.ajax.prototype.commands.submitBflRolesToForm = function(ajax, response, status) {
    var e=$.Event('keyup');
    e.which = 32;
   var updated_val = response.selectedValue;
    updated_val = updated_val.replace(/,\s*$/,'');
    temp_value = updated_val.split(",");
    var unique = temp_value.filter( onlyUnique );
    $(response.selector).val(unique).focus().trigger(e);
  };
  /* Utility function for filtering unique values */
  function onlyUnique(value, index, self) { 
    return self.indexOf(value) === index;
  }
}(jQuery, Drupal));