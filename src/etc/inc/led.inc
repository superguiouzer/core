<?php

/*
 * (C) 2009 Jim Pingle <jimp@pfsense.org>
 *
 * LED control library that wraps around the functionality of led(4)
 */

$led_root = "/dev/led/led";

/*
 * Send the control string to an LED
 */
function led_ctl($led, $str)
{
    global $led_root;
    if (led_exists($led)) {
        exec("/bin/echo " . escapeshellarg($str) . " > {$led_root}{$led}");
        return true;
    }
    return false;
}

/*
 * Blink an LED in a specific pattern
 * Letters A-J are on from 1/10s to 1s
 * Letters a-j are off from 1/10s to 1s
 */
function led_pattern($led, $pattern, $repeat=true)
{
    /*  End with a . to stop after one iteration. */
    $end = $repeat ? "" : ".";
    return led_ctl($led, "s{$pattern}{$end}");
}

/*
 * Encode a text message into morse code, and send it to an LED
 */
function led_morse($led, $message)
{
    return led_ctl($led, "m" . str_to_morse($message));
}


/*
 * Turn an LED on
 */
function led_on($led)
{
    led_ctl($led, "1");
}

/*
 * Turn an LED off
 */
function led_off($led)
{
    led_ctl($led, "0");
}

/*
 * Find the number of LEDs present on the system.
 */
function led_count() {
    global $led_root;
    $count = 0;
    $leds = array();
    if (is_dir(dirname($led_root))) {
        $leds = glob("{$led_root}*");
        $count = count($leds);
    }
    return $count;
}

/*
 * Test to see if a given LED exists.
 */
function led_exists($led) {
    global $led_root;
    if (!is_numeric($led)) {
        return false;
    }
    return file_exists("{$led_root}{$led}");
}

/*
 * Sweep across three LEDs in a K.I.T.T.-like way.
 */
function led_kitt()
{
    led_pattern(1, 'AaaaaA');
    led_pattern(2, 'aAaaAa');
    led_pattern(3, 'aaAAaa');
}

/*
 * Custom pattern for assigning interfaces
 */
function led_assigninterfaces()
{
    led_pattern(1, 'AaaAaaaaaaaaaaaa');
    led_pattern(2, 'aaaaaAaaAaaaaaaa');
    led_pattern(3, 'aaaaaaaaaaAaaAaa');
}

/*
 * Return the three LEDs to a standard setup (1=on, 2 and 3 = off)
 */
function led_normalize()
{
    led_on(1);
    led_off(2);
    led_off(3);
}

/*
 * Translate a string to morse code. Characters not known to have a
 * valid morse code representation will be ignored.
 */
function str_to_morse($string)
{
    $i = 0;
    $morsestring = "";
    while ($i < strlen($string)) {
        $morsestring .= char_to_morse(substr($string, $i++, 1)) . " ";
    }
    return $morsestring . "\n";
}

/*
 * Translate a single character to morse code. Characters not known
 * to have a valid morse code representation will be ignored.
 */
function char_to_morse($char)
{
    switch (strtoupper($char)) {
        case "A":
            return ".-";
            break;
        case "B":
            return "-...";
            break;
        case "C":
            return "-.-.";
            break;
        case "D":
            return "-..";
            break;
        case "E":
            return ".";
            break;
        case "F":
            return "..-.";
            break;
        case "G":
            return "--.";
            break;
        case "H":
            return "....";
            break;
        case "I":
            return "..";
            break;
        case "J":
            return ".---";
            break;
        case "K":
            return "-.-";
            break;
        case "L":
            return ".-..";
            break;
        case "M":
            return "--";
            break;
        case "N":
            return "-.";
            break;
        case "O":
            return "---";
            break;
        case "P":
            return ".--.";
            break;
        case "Q":
            return "--.-";
            break;
        case "R":
            return ".-.";
            break;
        case "S":
            return "...";
            break;
        case "T":
            return "-";
            break;
        case "U":
            return "..-";
            break;
        case "V":
            return "...-";
            break;
        case "W":
            return ".--";
            break;
        case "X":
            return "-..-";
            break;
        case "Y":
            return "-.--";
            break;
        case "Z":
            return "--..";
            break;
        case "0":
            return "-----";
            break;
        case "1":
            return ".----";
            break;
        case "2":
            return "..---";
            break;
        case "3":
            return "...--";
            break;
        case "4":
            return "....-";
            break;
        case "5":
            return ".....";
            break;
        case "6":
            return "-....";
            break;
        case "7":
            return "--...";
            break;
        case "8":
            return "---..";
            break;
        case "9":
            return "----.";
            break;
        case ".":
            return ".-.-.-";
            break;
        case ",":
            return "--..--";
            break;
        case "?":
            return "..--..";
            break;
        case "'":
            return ".----.";
            break;
        case "!":
            return "-.-.--";
            break;
        case "/":
            return "-..-.";
            break;
        case "(":
            return "-.--.";
            break;
        case ")":
            return "-.--.-";
            break;
        case "&":
            return ".-...";
            break;
        case ":":
            return "---...";
            break;
        case ";":
            return "-.-.-.";
            break;
        case "=":
            return "-...-";
            break;
        case "+":
            return ".-.-.";
            break;
        case "-":
            return "-....-";
            break;
        case "_":
            return "..--.-";
            break;
        case "$":
            return "...-..-";
            break;
        case "@":
            return ".--.-.";
            break;
        case '"':
            return ".-..-.";
            break;
        default:
            return "";
            break;
    }
}

?>
