// Register a template definition set named "default".
CKEDITOR.addTemplates('default',
        {
            // The name of the subfolder that contains the preview images of the templates.
            imagesPath: Drupal.settings.basePath + 'sites/all/themes/adminimal_theme/ckeditor-tpl/',
            // Template definitions.
            templates:
                    [
                        {
                            title: 'Text w/img aligned right',
                            image: 'imgright.png',
                            description: 'Image right',
                            html: Drupal.settings.adminimal.template_imageright
                        },
                        {
                            title: 'Text w/img aligned left',
                            image: 'imgleft.png',
                            description: 'Image left',
                            html: Drupal.settings.adminimal.template_imageleft
                        },
                        {
                            title: '1 column bulleted list',
                            image: '1bullets.png',
                            description: 'One bullet',
                            html: Drupal.settings.adminimal.template_1bullets
                        },
                        {
                            title: '2 column bulleted list',
                            image: '2bullets.png',
                            description: '2 Bullets',
                            html: Drupal.settings.adminimal.template_2bullets
                        },
                        {
                            title: '3 column bulleted list',
                            image: '3bullets.png',
                            description: '3 bullets',
                            html: Drupal.settings.adminimal.template_3bullets
                        },
                        {
                            title: '1 column text/links/img',
                            image: 'one.png',
                            description: 'One Column Layout',
                            html: Drupal.settings.adminimal.template_one
                        },
                        {
                            title: '2 column text/links/img',
                            image: 'two.png',
                            description: 'Two Column Layout',
                            html: Drupal.settings.adminimal.template_two
                        },
                        {
                            title: '3 column text/links/img',
                            image: 'three.png',
                            description: 'Three Column Layout',
                            html: Drupal.settings.adminimal.template_three
                        },
                        {
                            title: '4 column text/links/img',
                            image: 'four.png',
                            description: 'Four Column Layout',
                            html: Drupal.settings.adminimal.template_four
                        },
                        {
                            title: 'Table',
                            image: 'table.png',
                            description: 'Table Layout',
                            html: Drupal.settings.adminimal.template_table
                        },
                        {
                            title: 'Sidebar - Left ',
                            image: 'left_menu_bodytext.png',
                            html: Drupal.settings.adminimal.template_sidebar_left
                        },
                        {
                            title: 'Sidebar - Right ',
                            html: Drupal.settings.adminimal.template_sidebar_right
                        },
                        {
                            title: 'Well',
                            image: 'well_text.png',
                            html: Drupal.settings.adminimal.template_well
                        },
                        {
                            title: 'Quotation',
                            image: 'quotation_text.png',
                            html: Drupal.settings.adminimal.template_quotes
                        },
                        {
                            title: 'Structured List',
                            image: 'structured_list.png',
                            html: Drupal.settings.adminimal.template_structured_list
                        },
                        {
                            title: 'Inline list',
                            html: Drupal.settings.adminimal.template_inline_list
                        },
                        {
                            title: 'Description list',
                            image: 'description_list.png',
                            html: Drupal.settings.adminimal.template_description_defination
                        },
						{
                            title: 'Flipper',
                            html: Drupal.settings.adminimal.template_flipper
                        },
                        {
                            title: 'Horizontal description list ',
                            image: 'description_list_horiz.png',
                            html: Drupal.settings.adminimal.template_horizontal_description_list
                        },
						
                    ]
        });