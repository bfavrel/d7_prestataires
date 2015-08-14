(function ($) {
    $(document).ready(function(){

        var now = moment().format('YYYY-MM-DD');

        $('#edit-calendar').DatePicker({
            flat: true,
            calendars: 12,
			mode: 'range',
            view: 'days',
            format: 'Y-m-d',
            current: now,
            columns: 3,
            navigation: false,
            onRender: function(date) {
                var render = {
                    selected: false,
                    disabled: false,
                    className: []
                };

                var formated = moment(date).format('YYYY-MM-DD');

                if(formated == now) {
                    render.className = ['today'];
                } else if(formated < now) {
                    render.disabled = true;
                }

                if(window.calendarData['classes'][formated] != undefined) {
                    render.className.push(window.calendarData['classes'][formated].join(' '));
                }

                return render;
            },
            onChange: function(formated, has_infos){
                updateDatesElements(formated);
                getDateInformations(formated, has_infos);
            }
        });

        // dates : 'from' et 'to'
        function updateDatesElements(dates) {
            $('span#date_from').text(moment(dates[0]).format("DD/MM/YYYY"));

            if(dates[1] != dates[0]) {

                $('span#date_to').data('defaultMessage', $('span#date_to').text());
                $('span#date_to').text(moment(dates[1]).format("DD/MM/YYYY"));
                $('span#date_advice').fadeIn();

            } else if($('span#date_to').data('defaultMessage') != '') {

                $('span#date_to').text($('span#date_to').data('defaultMessage'));
                $('span#date_advice').fadeOut();
            }

            $('input[name="data[period][date_from]"]').val(dates[0]);
            $('input[name="data[period][date_to]"]').val(dates[1]);
        }

        // types de dispos : change
        $('input[name="data[type]"]').change(function(){
            dispoTypeCheck();
        });

        // types de dispos : check
        function dispoTypeCheck() {
            $('#prestataires-dispos-node-form input[name="data[type]"]').siblings('label').removeClass('selected');
            $('#prestataires-dispos-node-form input[name="data[type]"]:checked').siblings('label').addClass('selected');

            var value = $('#prestataires-dispos-node-form input[name="data[type]"]:checked').val();

            if(value != 1 && value != 2) {
                $('#prestataires-dispos-node-form fieldset#edit-data-infos').filter(':visible').slideUp();
            } else {
                $('#prestataires-dispos-node-form fieldset#edit-data-infos').filter(':hidden').slideDown();
            }
        }
        dispoTypeCheck();

        function getDateInformations(dates, has_infos) {

            // on n'intervient que sur le click de la date de début
            if(dates[1] != dates[0]) {return;}

            if($('input[name="data[pin_infos]"]').is(':checked') == true) {return;}

            $('form#prestataires-dispos-node-form div.ajax_loading').fadeTo('fast', 0.8);

            $.ajax({
                url: Drupal.settings['basePath'] + 'dispos/ajax/infos/' + window.calendarData['nid'] + '/edit/' + dates[0],
                type: 'POST',
                success: function(data) {

                    $('input[name="data[type]"]').removeAttr('checked');
                    $('input[name="data[type]"][value="' + data['dispo_type'] + '"]').attr('checked', 'checked');
                    dispoTypeCheck();

                    $('fieldset#edit-data-infos div.so_forms_form').replaceWith(data['infos']);
                    $('#prestataires-dispos-node-form div.ajax_loading').fadeOut('fast');
                    $('#prestataires-dispos-node-form div.ajax_loading').hide(); // si les infos sont cachées, le fadeOut ne se lance pas.
                }
            });
        }

    });
})(jQuery);