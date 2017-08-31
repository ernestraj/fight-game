(function($, Drupal)
{    Drupal.behaviors.customctools = {
        attach: function(context, settings) {
            $(document).ready(function(e) {
                var checkboxes = '#modal-content input[name="entity_ids[]"]';
                $(checkboxes).change(function() {
                    if (this.checked) {
                        var myvalue = $(this).val();
                        $('#edit-enter-role option[value="' + myvalue + '"]').attr('selected', 'selected');
                        $('#edit-enter-role').chosen().change();
                        $('#edit-enter-role').trigger('liszt:updated');
                    }
                });
            });
        }
    }
    // Our function name is prototyped as part of the Drupal.ajax namespace, adding to the commands:
    Drupal.ajax.prototype.commands.insertRole = function(ajax, response, status)
    {
        // The value we passed in our Ajax callback function will be available inside the
        // response object. Since we defined it as selectedValue in our callback, it will be
        // available in response.roles.
        var e = $.Event('keyup');
        e.which = 32;
        var old = $(response.selector).val();

        var newstr = response.roles;
        var update = old.concat(newstr);

        temp = update.split(",");
        function onlyUnique(value, index, self) {
            return self.indexOf(value) === index;
        }
        var unique = temp.filter(onlyUnique);
        $(response.selector).val(unique).focus().trigger(e);

    };
}(jQuery, Drupal));