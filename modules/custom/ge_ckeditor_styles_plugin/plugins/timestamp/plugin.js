/**
 * Basic sample plugin inserting current date and time into CKEditor editing area.
 */

//for easy does it, we'll wrap the plugin inside a closure with jQuery
(function($) {
    CKEDITOR.plugins.add('timestamp', {
        icons: 'timestamp',
        init: function(editor) {
            var config = editor.config;

            function getHtml($element) {
                return $element.appendTo('<p></p>').parent().html();
            }
            // Gets the list of insertable strings from the settings.
            var style = config.custom_styles;
            // add the menu to the editor
            var generateElements = function(name, elements, attributes, output) {
                var html = '';
                output = output || false;
                if (output !== false) {
                    html = typeof name === 'object' ? getHtml(name) : name;
                } else {
                    html = $('<p>' + name + '</p>').text()
                }
                var currElement = '', finalElements = [];
                for (var e in elements) {
                    var index = elements[e];
                    currElement = $(document.createElement(index));
                    var attrs = attributes[index];
                    for (var attr in attrs) {
                        currElement.attr(attr, attrs[attr]);
                    }
                    finalElements[e] = currElement;
                }

                for (var f = finalElements.length - 1; f > 0; f -= 1) {
                    finalElements[f].appendTo(finalElements[f - 1]);
                }
                var innerHtml = finalElements[0].html();
                finalElements[0].html(html + innerHtml);
                return getHtml(finalElements[0]);

            }
            editor.ui.addRichCombo('timestamp', {
                label: config.strinsert_button_label,
                title: config.strinsert_button_title,
                voiceLabel: config.strinsert_button_voice,
                toolbar: 'insert',
                className: 'cke_format',
                multiSelect: false,
                panel: {
                    css: [editor.config.contentsCss, CKEDITOR.skin.getPath('editor')],
                    voiceLabel: editor.lang.panelVoiceLabel
                },
                init: function() {
                    for (var i = 0, len = style.length; i < len; i++) {
                        var combo = style[i];
                        var label = generateElements(combo.name, combo.elements, combo.attributes);
//                        var elements = generateElements(combo.name, combo.elements, combo.attributes);
//                        var label = getHtml(elements[0]);
//                        var name = $('<p>' + combo.name + '</p>').text();
                        // If there is no value, make a group header using the name.
                        if (!combo.elements) {
                            this.startGroup(combo.name);
                        } else {
                            this.add(i, label, name);
                        }
                    }
                },
                onClick: function(value) {
                    editor.focus();
                    var combo = config.custom_styles[value];
                    var selection = editor.getSelection();
                    var currentData = selection.getSelectedText();
                    var output = generateElements(currentData, combo.elements, combo.attributes, true);
                    editor.insertElement(CKEDITOR.dom.element.createFromHtml(output));
//                    for (var o in output) {
//                        console.log(getHtml(output[o]));
//                        editor.insertElement(CKEDITOR.dom.element.createFromHtml(getHtml(output[o])));
//                    }
//                    debugger;
//                    editor.getSelection().selectBookmarks(bookmarks);
//                    editor.insertElement(CKEDITOR.dom.element.createFromHtml('<i class=""> i </i>'));

                },
            });
        }
    });
})(jQuery);