/* 
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

myjQuery(document).ready(function() {
    var options = myjQuery("[id*='edit-field-menu-']");
    options.each(function() {
        myjQuery(this).selectric({
            onClose: function() {
                clearLabel(myjQuery(".selectric p.label"));
            }
        });
        clearLabel(myjQuery(".selectric p.label"));
    });

    myjQuery(".selectric").each(function() {
        myjQuery(this).next('.selectricItems').find('ul li').not(':first').each(function() {
            var opt = myjQuery(this);
            var txt = opt.text().replace(/^\s+|\s+$/gm, '');
            var matches = (txt.match(/^-+/g) || []);
            if (matches.length > 0) {
                var count = matches[0].split("-").length - 1;
                opt.addClass("level" + count);
                opt.css("font-weight", "normal");
                opt.text(opt.text().replace(/^-+/ig, ""));
            } else {
                opt.addClass("bold");
            }
        });
    });

    function clearLabel(object) {
        var label = object.text();
        label = myjQuery.trim(label).replace(/^-+/g, '');
        object.text(label);
        return object;
    }
});

