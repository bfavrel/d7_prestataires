(function ($) {
    $(document).ready(function(){

        var now = moment().format('YYYY-MM-DD');

        $('#edit-calendar-wrapper-calendar-calendar').DatePicker({
            flat: true,
            calendars: 1,
            months: 12,
			mode: 'single',
            view: 'days',
            format: 'Y-m-d',
            current: now,
            columns: 3,
            navigation: true,
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
                getDateInformations(formated, has_infos);
            }
        });

        function getDateInformations(date, has_infos) {

            if(has_infos == false) {return;}

            $('form#prestataires-dispos-calendar-page-form div.ajax_loading').fadeTo('fast', 0.8);

            $.ajax({
                url: Drupal.settings['basePath'] + 'dispos/ajax/infos/' + window.calendarData['nid'] + '/page/' + date,
                type: 'POST',
                success: function(data) {

                    $('div#calendar_infos').html(data);
                    $('div#calendar_infos_wrapper').fadeIn();
                }
            });
        }

        function closeInfos(evt) {
            evt.preventDefault();

            $('div#calendar_infos_wrapper').fadeOut('fast');
            $('form#prestataires-dispos-calendar-page-form div.ajax_loading').fadeOut('fast');
        }

        $('#calendar_infos_wrapper a#close_infos').click(closeInfos);
        $('form#prestataires-dispos-calendar-page-form div.ajax_loading').click(closeInfos);

    });
})(jQuery);

