<?php
/**
 * Theme the calendar's node elements.
 *
 *  - $element['calendar'] : the calendar and its JS data
 *  - $element['calendar_updated'] : the calendar last update text
 *  - $element['calendar_legend'] : calendar's colors legend
 */

print("<h2>" . t("Availabilities") . "</h2>");

print(render($element['calendar']));

print(render($element['calendar_updated']));

print(render($element['calendar_legend']));
