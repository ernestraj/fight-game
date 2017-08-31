/*
Copyright (c) 2003-2013, CKSource - Frederico Knabben. All rights reserved.
For licensing, see LICENSE.html or http://ckeditor.com/license
*/

/*
 WARNING: clear browser's cache after you modify this file.
 If you don't do this, you may notice that browser is ignoring all your changes.
 */
 

CKEDITOR.editorConfig = function(config) {

  config.templates_files = [ Drupal.settings.basePath + 'sites/all/themes/adminimal_theme/mytemplates.js' ]; 
  config.indentClasses = [ 'rteindent1', 'rteindent2', 'rteindent3', 'rteindent4' ];

  // [ Left, Center, Right, Justified ]
  config.justifyClasses = [ 'rteleft', 'rtecenter', 'rteright', 'rtejustify' ];

  // The minimum editor width, in pixels, when resizing it with the resize handle.
     config.resize_minWidth = 450;
     config.disallowedContent = 'img{width,height}';
     CKEDITOR.on('dialogDefinition', function (ev) {

        // Take the dialog name and its definition from the event data.
        var dialogName = ev.data.name;
        var dialogDefinition = ev.data.definition;
        // Check if the definition is from the dialog we're
        // interested in (the 'image' dialog).
        if (dialogName == 'image') {
            // Get a reference to the 'Image Info' tab.
            var infoTab = dialogDefinition.getContents('info');
            // Remove unnecessary widgets/elements from the 'Image Info' tab.
           // infoTab.remove('browse');
           // infoTab.remove('txtHSpace');
            //infoTab.remove('txtVSpace');
            //infoTab.remove('txtBorder');
            infoTab.remove('txtAlt');
            infoTab.remove('txtWidth');
            infoTab.remove('txtHeight');
            infoTab.remove('htmlPreview');
            infoTab.remove('cmbAlign');
            infoTab.remove('ratioLock');
        } // Added script for the remove Link target tab options values 
            else if (ev.data.name === 'link') {
        var target = ev.data.definition.getContents('target');
        var options = target.get('linkTargetType').items;
        for (var i = options.length-1; i >= 0; i--) {
              var label = options[i][1];
               if ((label !== "notSet")) {
                  if(label !=="_blank"){
                options.splice(i, 1)
            }
          }
        }
        var targetField = target.get( 'linkTargetType' );
        //targetField['default'] = '_blank';
    }

		
    });
	CKEDITOR.on('instanceReady', function (ev) {
// Ends self closing tags the HTML4 way, like <br>.
ev.editor.dataProcessor.htmlFilter.addRules(
    {
        elements:
        {
            $: function (element) {
                // Output dimensions of images as width and height
                if (element.name == 'img') {
                    var style = element.attributes.style;

                    if (style) {
                        // Get the width from the style.
                        var match = /(?:^|\s)width\s*:\s*(\d+)px/i.exec(style),
                            width = match && match[1];

                        // Get the height from the style.
                        match = /(?:^|\s)height\s*:\s*(\d+)px/i.exec(style);
                        var height = match && match[1];

                        if (width) {
                            element.attributes.style = element.attributes.style.replace(/(?:^|\s)width\s*:\s*(\d+)px;?/i, '');
                            element.attributes.width = width;
                        }

                        if (height) {
                            element.attributes.style = element.attributes.style.replace(/(?:^|\s)height\s*:\s*(\d+)px;?/i, '');
                            element.attributes.height = height;
                        }
                    }
                }



                if (!element.attributes.style)
                    delete element.attributes.style;
					delete element.attributes.width;
					delete  element.attributes.height;

                return element;
            }
        }
    });
});
   
  /**
  * Custom Styles plugin configuration, the array structure should be as follows:
  * name: the name to be placed on the drop down texts.
  * elements: an array with the element's tag names to be added.
  * attributes: an object containing attributes to be added to the element,
  *  Every Key on the object must match the corresponding tag on the elements array
  * {
  *   'name' : 'My title Name',
  *   'elements': ['span', 'i', 'h3'],
  *   'attributes': {
  *       'span' : {
  *            'class' : 'some_class',
  *            'id' : 'some_id',
  *            'etc' : 'bla bla'
  *       },
  *       'i' : {
  *            'class' : 'some_class',
  *            'id' : 'some_id',
  *            'etc' : 'bla bla'
  *       }
  *   }
  */
config.custom_styles = [
        {'name': 'Section Title', 'elements': ['h3'], 'attributes': {'h3': {'class': 'ge-element sectionTitle'}} },
		{'name': 'Sub-section Title', 'elements': ['h4'], 'attributes': {'h4': {'class': 'ge-element sub-sectionTitle'}}},
		{'name': 'Action Arrow Link', 'elements': ['span', 'i'], 'attributes': {'span': {'class': 'ge-element btn btn-link arrow'}, 'i': {'class': 'add-arrow-right'}}},
		{'name': 'Button Link', 'elements': ['span'], 'attributes': {'span': {'class': 'ge-element btn'}}},
		{'name': 'list Link', 'elements': ['span'], 'attributes': {'span': {'class': 'ge-element btn btn-link'}}},
		{'name': 'Folder Open', 'elements': ['i'], 'attributes': {'i': {'class': 'icon-ico_folderopen_sm'}}},
		{'name': 'External Link', 'elements': ['i'], 'attributes': {'i': {'class': 'icon-ico_externallink_sm'}}},
		{'name': 'Page sheet', 'elements': ['i'], 'attributes': {'i': {'class': 'icon-ico_documents_sm'}}},
		{'name': 'Timestamp', 'elements': ['span'], 'attributes': {'span': {'class': 'ge-element timestamp'}}}
    ];
CKEDITOR.config.coreStyles_italic =
    {   element : 'i',
        };
	

  // Protect PHP code tags (<?...?>) so CKEditor will not break them when
  // switching from Source to WYSIWYG.
  // Uncommenting this line doesn't mean the user will not be able to type PHP
  // code in the source. This kind of prevention must be done in the server
  // side
  // (as does Drupal), so just leave this line as is.
  config.protectedSource.push(/<\?[\s\S]*?\?>/g); // PHP Code
  config.protectedSource.push(/<code>[\s\S]*?<\/code>/gi); // Code tags
  // ALLOW <i></i>
//config.protectedSource.push( /<i class[\s\S]*?\>/g ); //allows beginning <i> tag
//config.protectedSource.push( /<\/i>/g ); //allows ending </i> tag
CKEDITOR.dtd.$removeEmpty['i'] = false;
config.extraPlugins = '';
config.allowedContent = true;
//config.extraAllowedContent = 'i(arrow,ge-element)';

  /*
    * Append here extra CSS rules that should be applied into the editing area.
    * Example:
    * config.extraCss = 'body {color:#FF0000;}';
    */
  config.extraCss = '';

  /**
    * Sample extraCss code for the "marinelli" theme.
    */
  if (Drupal.settings.ckeditor.theme == "marinelli") {
    config.extraCss += "body{background:#FFF;text-align:left;font-size:0.8em;}";
    config.extraCss += "#primary ol, #primary ul{margin:10px 0 10px 25px;}";
  }
  if (Drupal.settings.ckeditor.theme == "newsflash") {
    config.extraCss = "body{min-width:400px}";
  }

  /**
    * CKEditor's editing area body ID & class.
    * See http://drupal.ckeditor.com/tricks
    * This setting can be used if CKEditor does not work well with your theme by default.
    */
  config.bodyClass = '';
  config.bodyId = '';
  /**
    * Sample bodyClass and BodyId for the "marinelli" theme.
    */
  if (Drupal.settings.ckeditor.theme == "marinelli") {
    config.bodyClass = 'singlepage';
    config.bodyId = 'primary';
  }
}

/*
 * Sample toolbars
 */

//Toolbar definition for basic buttons
Drupal.settings.cke_toolbar_DrupalBasic = [ [ 'Format', 'Bold', 'Italic', '-', 'NumberedList','BulletedList', '-', 'Link', 'Unlink', 'Image' ] ];

//Toolbar definition for Advanced buttons
Drupal.settings.cke_toolbar_DrupalAdvanced = [
  ['Source'],
  ['Cut','Copy','Paste','PasteText','PasteFromWord','-','SpellChecker', 'Scayt'],
  ['Undo','Redo','Find','Replace','-','SelectAll','RemoveFormat'],
  ['Image','Flash','Table','HorizontalRule','Smiley','SpecialChar'],
  ['Maximize', 'ShowBlocks'],
  '/',
  ['Format'],
  ['Bold','Italic','Underline','Strike','-','Subscript','Superscript'],
  ['NumberedList','BulletedList','-','Outdent','Indent','Blockquote'],
  ['JustifyLeft','JustifyCenter','JustifyRight','JustifyBlock','-','BidiRtl','BidiLtr'],
  ['Link','Unlink','Anchor','Linkit','LinkToNode','LinkToMenu'],
  ['DrupalBreak', 'DrupalPageBreak']
];

// Toolbar definiton for all buttons
Drupal.settings.cke_toolbar_DrupalFull = [
  ['Source'],
  ['Cut','Copy','Paste','PasteText','PasteFromWord','-','SpellChecker', 'Scayt'],
  ['Undo','Redo','Find','Replace','-','SelectAll','RemoveFormat'],
  ['Image','Flash','Table','HorizontalRule','Smiley','SpecialChar','Iframe'],
  '/',
  ['Bold','Italic','Underline','Strike','-','Subscript','Superscript'],
  ['NumberedList','BulletedList','-','Outdent','Indent','Blockquote','CreateDiv'],
  ['JustifyLeft','JustifyCenter','JustifyRight','JustifyBlock','-','BidiRtl','BidiLtr'],
  ['Link','Unlink','Anchor','Linkit','LinkToNode', 'LinkToMenu'],
  '/',
  ['Format','Font','FontSize'],
  ['TextColor','BGColor'],
  ['Maximize', 'ShowBlocks'],
  ['DrupalBreak', 'DrupalPageBreak']
];

