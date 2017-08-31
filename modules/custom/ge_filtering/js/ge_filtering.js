/**
 * @File for all js customizations related to project.
 */

(function ($) {
  Drupal.behaviors.ge_filtering = {
    attach: function (context, settings) {
      $(document).ready(function (e) {	
	  if(typeof(CKEDITOR) !== 'undefined'){
    CKEDITOR.on('dialogDefinition', function(ev) {
	var editor = ev.editor;
	var dialogName = ev.data.name;
	var dialogDefinition = ev.data.definition;
	if (dialogName == 'link') {
		  var infoTab = dialogDefinition.getContents( 'info' );
		var linkType = infoTab.get( 'linkType' ); 
		linkType['onMyEvent'] = linkType['onChange'];
		linkType['onChange'] = function(evt){
		    $(this).trigger( 'onMyEvent' );
			var s=this.getValue();
			if(s=='url'){
			var dialog = this.getDialog();
		    var t='_blank';
		    dialog.setValueOf('target', 'linkTargetType', t);
			}
			if(s=='drupal'){
				var dialog = this.getDialog();
		    var t='notSet';
				   dialog.setValueOf('target', 'linkTargetType', t);
			}
	  }}});	
	}	  
        if (($('body').hasClass('page-node-add-news-article') || $('body').hasClass('node-type-news-article'))){
          var type = Drupal.settings.ge_filtering.type;
          var news_type = Drupal.settings.ge_filtering.news_type;
          var country = Drupal.settings.ge_filtering.country;
          if(type == 'administrator' || $('#news-article-node-form #edit-field-news-type-und').val() == news_type) {
            $(".vertical-tabs").show();
            $(".vertical-tabs .form-item-log").hide();
          }
          else if(type !== 'administrator' && country == undefined) {
            $(".vertical-tabs").hide();
          }
        }
        $('#news-article-node-form #edit-field-news-type-und').change(function () {
          var news_article_id = $(this).attr('id');
          if ($("#" + news_article_id + " option:selected").text() == "New Stories" 
            || $("#" + news_article_id + " option:selected").text() == "New Stories (List)") {
            $("#edit-field-description").hide();
          }
          else {
            $("#edit-field-description").show();
          }
          if(type !== 'administrator') {
            if ($("#" + news_article_id + " option:selected").val() !== news_type && country == undefined) {
              $(".vertical-tabs").hide();
              $(".vertical-tabs .form-item-log").hide();
              $('#edit-workbench-moderation-state-new option[value="draft"]').attr('selected', true);
            }
            else {
              $(".vertical-tabs").show();
              $(".vertical-tabs .form-item-log").hide();
            }
          }
        });
                if ($("#views-form-projects-page-1 #edit-actions #edit-submit").val() == "Next") {
                    $("#content").hide();
                    $('#edit-submit').click();
                }
                $('.field-name-field-carousel-interval').hide();
                $('.field-name-field-carousel-settings').hide();
                $('.field-name-field-section-type.field-widget-options-select').change(function (e) {
                    var id_carousel = _get_element_id($(this).attr('id'));
                    var current_id = $(this).attr('id');
                    if ($("#" + current_id + " option:selected").text() == "Carousel") {
                        $('#' + id_carousel[0]).show();
                        $('#' + id_carousel[1]).show();
                    }
                    else {
                        $('#' + id_carousel[0]).hide();
                        $('#' + id_carousel[1]).hide();
                    }
                });

                $('.field-type-taxonomy-term-reference.field-name-field-link-style').change(function (e) {
                    if ($("#" + $(this).attr('id') + " option:selected").text() == "Carousel") {
                        $(this).siblings(".field-name-field-carousel-interval").show();
                        $(this).siblings(".field-name-field-carousel-settings").show();
                    }
                    else {
                        $(this).siblings(".field-name-field-carousel-interval").hide();
                        $(this).siblings(".field-name-field-carousel-settings").hide();
                    }
                });

                $('.field-name-field-link-style.field-widget-options-select').change(function (e) {
                    var link_carousel = _get_link_id($(this).attr('id'));
                    var carousel_id = $(this).attr('id');
                    if ($("#" + carousel_id + " option:selected").text() == "Carousel") {
                        $('#' + link_carousel[0]).show();
                        $('#' + link_carousel[1]).show();
                    }
                    else {
                        $('#' + link_carousel[0]).hide();
                        $('#' + link_carousel[1]).hide();
                    }
                });

                $('.shown-sections.user-sections ul').hide();
                 $('.shown-sections.user-sections span').click(function (e) {
                    $(this).toggleClass('active');
                    $('.shown-sections.user-sections ul').slideUp();
                    if ($(this).next('ul').is(':visible')) {
                        $(this).next('ul').slideUp();
                    }
                    else {
                        $(this).next('ul').slideDown();
                    }
                    $(this).next('ul').toggleClass('active-items');
                });
                $("#edit-rules-componentrules-change-project-name").hide();
                $('#edit-assign-project').click(function (e) {
                    if ($("#edit-project-selected").val() == 0) {
                        alert("Please select a project first");
                    }
                    else {
                        $("#edit-rules-componentrules-change-project-name").click();
                    }
                    return false;
                });
                $('#archive-project-list-link').click(function (e) {
                    $("#block-views-project-list-page-block").slideUp('slow');
                    $("#view-project-list-link").text("View Live Projects");
                    if ($("#block-views-project-list-page-block-1").is(":visible")) {
                        $("#block-views-project-list-page-block-1").slideUp('slow');
                        $(this).text("View Archived Projects");
                    }
                    else {
                        $("#block-views-project-list-page-block-1").slideDown('slow');
                        $(this).text("Hide Archived Projects");
                    }
                    return false;
                });

                $("#view-project-list-link").click(function (e) {
                    $("#block-views-project-list-page-block-1").slideUp('slow');
                    $("#archive-project-list-link").text("View Archived Projects");
                    if ($(".view-id-project_list_page.view-display-id-block .view-content").is(":visible")) {
                        $("#block-views-project-list-page-block").slideUp('slow');
                        $(this).text("View Live Projects");
                    }
                    else {
                        $("#block-views-project-list-page-block").slideDown('slow');
                        $(this).text("Hide Live Projects");
                    }
                    return false;
                });
                //Hide all translation links on Page load
                $(".translation-links").each(function() {
                  if($(this).next('div').find('div.item-list').html() == '') {
                    $(this).hide();
                  } else {
                    $(".translation-links").next('div').hide();
                  }
                });
                // Show/hide translation links on click of Translation links
                $(".translation-links").click(function (e) {
                    e.preventDefault();
                    if($(this).next('div').css('display') == 'none') {
                      $(this).find('a').html("Hide Translation Links");
                    } else if($(this).next('div').css('display') == 'block') {
                      $(this).find('a').html("View Translation Links");
                    }
                    $(this).next('div').slideToggle('down');
                });
				//Hide all Portal name on Page load 
                $(".views-country-portal").each(function() {
                  if($(this).next('div').find('div.item-list').html() == '') {
                    $(this).hide();
                  } else {
                    $(".views-country-portal").next('div').hide();
                  }
                });
                // Show/hide Portal name on click of Portal Name
                $(".views-country-portal").click(function (e) {
                    e.preventDefault();
                    if($(this).next('div').css('display') == 'none') {
                      $(this).find('a').html("Hide Portal");
                    } else if($(this).next('div').css('display') == 'block') {
                      $(this).find('a').html("View Portal");
                    }
                    $(this).next('div').slideToggle('down');
                });
                //Hide all Sections name on Page load 
                $(".views-section").each(function() {
                  if($(this).next('div').find('div.item-list').html() == '') {
                    $(this).hide();
                  } else {
                    $(".views-section").next('div').hide();
                  }
                });
                 // Show/hide Section name on click of section name
                $(".views-section").click(function (e) {
                    e.preventDefault();
                    if($(this).next('div').css('display') == 'none') {
                      $(this).find('a').html("Hide Sections");
                    } else if($(this).next('div').css('display') == 'block') {
                      $(this).find('a').html("View Sections");
                    }
                    $(this).next('div').slideToggle('down');
                });
			  //word wrap for newsarticle
                $(".newsarticle").each(function() {	
                   if($(this).next('div').html() == '') {
                      $(this).hide();
                     }else{
					    var content = $(this).next('div').html();
						if(content.length > 50) {
                         var c = content.substr(0, 50);
                         var h = content.substr(50, content.length - 50);
                         var html = c + '<span class="moreellipses"> ... &nbsp;</span><span class="morecontentnewsarticle"><span>' + h + '</span>&nbsp;&nbsp;<a href="" class="morelinknewsarticle"> Show more </a></span>';
                         $(this).next('div').html(html);                                           
                        }}
                    });
				
				$(".morelinknewsarticle").click(function(e){
				    e.preventDefault();
                if($(this).hasClass("less")) {
                    $(this).removeClass("less");
                    $(this).html('Show more');
                    } else {
                    $(this).addClass("less");
                    $(this).html('Show less');
                    }
                $(this).parent().prev().slideToggle('down');
                $(this).prev().slideToggle('down');
                return false;
                });

            });

            function _get_link_id(id) {
                var split = id.split('-');
                var return_array = ['edit-field-group-und-' + split[4] + '-field-section-und-' + split[8] + '-field-carousel-interval', 'edit-field-group-und-' + split[4] + '-field-section-und-' + split[8] + '-field-carousel-settings']
                return return_array;
            }

            function _get_element_id(id) {
                var split = id.split('-');
                var return_array = ['edit-field-group-und-' + split[4] + '-field-carousel-interval', 'edit-field-group-und-' + split[4] + '-field-carousel-settings'];
                return return_array;
            }
        }
    };

    $('document').ready(function (e) {
        $('#block-views-project-list-page-block').hide();
        $('#block-views-project-list-page-block-1').hide();
    });
}(jQuery));
