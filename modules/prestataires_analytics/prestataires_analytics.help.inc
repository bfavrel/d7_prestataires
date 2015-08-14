<?php

function prestataires_analytics_help($path, $arg) {
    
    switch($path) {
        
        case 'admin/config/prestataires/settings/analytics':
            return "<p>" . t(
                "You can find detailed help !link for account settings.",
                array('!link' => l(t("here"), 'admin/help/prestataires_analytics', array('attributes' => array('target' => '_blank'))))
            ) . "</p>";
            break;
        
        case 'admin/help#prestataires_analytics':
            return t("
<h2>1/ Creating a service account</h2>
<p>A service account's credentials include a generated email address that is unique, a client ID, and at least one public/private key pair.</p>
<p>If your application runs on Google App Engine, a service account is set up automatically when you create your project.</p>
<p>If your application doesn't run on Google App Engine or Google Compute Engine, you must obtain these credentials in the Google Developers Console. To generate service-account credentials, or to view the public credentials that you've already generated, do the following:</p>
<ol>
<li>Go to the <a href=\"https://console.developers.google.com/\">Google Developers Console</a>.</li>
<li> Select a project, or create a new one. </li>
<li>In the sidebar on the left, expand <strong>APIs & auth</strong>. Next, click <strong>APIs</strong>. Select the <strong>Enabled APIs</strong> link in the API section to see a list of all your enabled APIs. Make sure that the API is on the list of enabled APIs. If you have not enabled it, select the API from the list of APIs, then select the <strong>Enable API</strong>
button for the API.</li>
<li>In the sidebar on the left, select <strong>Credentials</strong>.</li>
<li><p>To set up a new service account, do the following:</p>
<ol><li>Under the <strong>OAuth</strong> heading, select <strong>Create new Client ID</strong>.</li>
<li>When prompted, select <strong>Service Account</strong> and click <strong>Create Client ID</strong>.</li>
<li>A dialog box appears. To proceed, click <strong>Okay, got it</strong>.</li>
</ol>
<p></li>
</ol>
<p>Your new Public/Private key pair is generated and downloaded to your machine; it serves as the only copy of this key. You are responsible for storing it securely. The Console shows your private key's password only at this initial moment of service account creation - the password will not be shown again ! -. You now have to 
<strong>Generate New P12 Key</strong> with the provided button. Also take note of the <strong>service account's email address</strong>
</p>
<h2>2/ Authorize service account's email address in Google Analytics</h2>
<p>In Google Analytics 'Administration > User Management', add the <strong>service account's email address</strong> on the analytics accounts you want to access and give it the 'Read and Analyse' permissions.</p>
<h2>3/ Get the profile ID in Google Analytics</h2>
<p><ol>
<li>Log into Goggle Analytics</li>
<li>Select the profile you want</li>
<li>
When the page has loaded, look at the URL – you are looking for the last part of the URL, a bit of text that
<em>should</em> start with an <strong>a</strong> and include a <strong>w </strong> and a <strong>p</strong> 
about 1/3 and 2/3 the way along. The text should look something like: aXXXXXXXwZZZZZZZZp12345678 where the X’s and Z’s are numbers and 12345678 is the
<strong>profile ID</strong> (also called \"<strong>report ID</strong>\").
</li>
</ol></p>
                ");
            break;
    }
}