(function ($, Drupal) {
    Drupal.behaviors.dynatree = {
        attach: function (context, settings) {
            $("#tree").ajaxComplete(function (event, XMLHttpRequest, ajaxOptions) {
                _log("debug", "ajaxComplete: %o", this); // dom element listening
            });
            var data = Drupal.settings['ge_bfl_roles_popup'];

            // --- Initialize sample trees
            $("#tree").dynatree({
                checkbox: true,
                clickFolderMode: 3,
                selectMode: 2,
                title: "Lazy loading sample",
                fx: {height: "toggle", duration: 200},
                autoFocus: false, // Set focus to first child, when expanding or lazy-loading.
                // In real life we would call a URL on the server like this:
                initAjax: {
                    url: Drupal.settings.basePath + "tree_with_ajax/" + data.vid
                },
                onPostInit: function (isReloading, isError) {

                },
                onFocus: function (node) {
                    var options = $("#edit-selected-bfl").chosen().val();
                    $.each(options, function (index, item) {
                        $("#tree").dynatree("getTree").getNodeByKey(item).select();
                    });
                },
                onExpand: function (node) {
                    var options = $("#edit-selected-bfl").chosen().val();
                    $.each(options, function (index, item) {
                        $("#tree").dynatree("getTree").getNodeByKey(item).select();
                    });
                },
                onActivate: function (node) {
                    $("#echoActive").text("" + node + " (" + node.getKeyPath() + ")");
                },
                onLazyRead: function (node) {
                    // In real life we would call something like this:
                    node.appendAjax({
                        url: Drupal.settings.basePath + "get_child/" + node.data.key,
                        success: function(data) {
                            if (node.data.checkboxClicked === true) {
                                //selectNodeChild(node, node.isSelected());
                                selectNodeChild(node, true);
                            }
                        }
                    });
                },
                onClick: function(node, event) {
                    var clickedTarget = node.getEventTargetType(event);
                    if(clickedTarget === "title" && node.data.isFolder){
                        // Not expanded by ajax.
                        if (node.childList === null) {
                            selectNodeChild(node, true);
                        }
                        else {
                            var child_selected = false;
                            for (var i = 0; i < node.childList.length; i++) {
                                var childNode = node.childList[i];
                                if (childNode.isSelected()) {
                                    child_selected = true;
                                    break;
                                }
                            }
                            selectNodeChild(node, !child_selected);
                        }
                        return false;// Prevent default processing (toggle operation)
                    }
                    else if(clickedTarget === "checkbox" && node.data.isFolder){
                        var level = node.getLevel();
                        if (level == 2) {
                            selectNodeChild(node, !node.isSelected());
                        }
                    }
                },
                onSelect: function (select, node) {
                    // Display list of selected nodes
                    var selNodes = node.tree.getSelectedNodes();
                    var aux = 0;
                    // convert to title/key array
                    var selKeys = $.map(selNodes, function (node) {
                        var index = selNodes.length;
                        return '"' + aux++ + '":{"tid":' + node.data.key + '}';
                    });
                    aux = 0;
                    $('#selected').text('{' + selKeys.join(",") + '}');
                    var myvalue = node.data.key;
                    //Make the checked option selected in chosen widget 
                    if (select) {
                        $('#edit-selected-bfl option[value="' + myvalue + '"]').attr('selected', 'selected');
                        $('#edit-selected-bfl').chosen().change();
                        $('#edit-selected-bfl').trigger('liszt:updated');
                    }
                    else {
                        $('#edit-selected-bfl option[value="' + myvalue + '"]').removeAttr('selected');
                        $('#edit-selected-bfl').chosen().change();
                        $('#edit-selected-bfl').trigger('liszt:updated');
                    }
                }
            });
            $("#btnReloadActive").click(function () {
                var node = $("#tree").dynatree("getActiveNode");
                if (node && node.isLazy()) {
                  node.reloadChildren(function (node, isOk) {
                    });
                } else {
                    alert("Please activate a lazy node first.");
                }
            });
        }
    }

    /**
     * Select/Unselect all the childs of the given node in dynatree.
     *
     * @param {object} node
     * @param {bool} op  (select => true | unselect => false)
     */
    function selectNodeChild(node, op) {
        // This JS setting is set in module so that it will work only for
        // location dynatree element (not on others).
        //var select_all_child = Drupal.settings.ge_bfl_roles_dynatree.child_select_all;

        // If select all child settings is set for field.
        var select_all_child = true;
        if (select_all_child) {

            // No don't need to do anything for the root('All') node.
            var level = node.getLevel();
            if (level == 1) {
                return;
            }

            // If node is not expanded, just expand it.
            var expanded = node.isExpanded();
            if (!expanded) {
                // Using settimeout because of race condition.
                setTimeout(function(){
                    node.expand();
                }, 1);
            }

            // Creating and initializing this variable so that in expand callback,
            // we able to select/de-select child of the given parent.
            node.data.checkboxClicked = true;

            // Select all the children of the given node.
            if (node.childList !== null) {
                for (var i = 0; i < node.childList.length; i++) {
                        var childNode = node.childList[i];
                        childNode.select(op);
                }
            }
        }
    }

}(jQuery, Drupal));
