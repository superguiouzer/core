<?php

/*
    Copyright (c) 2015-2016 Franco Fichtner <franco@opnsense.org>
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice,
       this list of conditions and the following disclaimer.

    2. Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
    INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
    AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
    AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
    OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.
*/

function legacy_interface_listget($flag = '')
{
    $cmd = '/sbin/ifconfig -l';
    $ifs = null;

    if ($flag === 'up') {
        $cmd .= 'u';
    } else if ($flag === 'down') {
        $cmd .= 'd';
    }

    exec($cmd . ' 2>&1', $out, $ret);
    if ($ret) {
        log_error('The command `' . $cmd . '\' failed to execute');
        return ($ifs);
    }

    if (isset($out[0])) {
        $ifs = explode(' ', $out[0]);
    }

    return ($ifs);
}

function legacy_interface_flags($ifs, $flag, $report_errors=true)
{
    /* $flags isn't escaped because it can be an argument list */
    $cmd = '/sbin/ifconfig ' . escapeshellarg($ifs) . ' ' . $flag;

    exec($cmd . ' 2>&1', $out, $ret);
    if (!empty($ret) && $report_errors) {
        log_error('The command `' . $cmd . '\' failed to execute');
    }
}

function legacy_interface_create($ifs)
{
    $cmd = '/sbin/ifconfig ' . escapeshellarg($ifs) . ' create';
    $new = null;

    exec($cmd . ' 2>&1', $out, $ret);
    if ($ret) {
        log_error('The command `' . $cmd . '\' failed to execute');
        return ($new);
    }

    if (isset($out[0])) {
        $new = $out[0];
    }

    return ($new);
}

function legacy_interface_destroy($ifs)
{
    $cmd = '/sbin/ifconfig ' . escapeshellarg($ifs) . ' destroy';

    exec($cmd . ' 2>&1', $out, $ret);
    if ($ret) {
        log_error('The command `' . $cmd . '\' failed to execute');
    }
}

function legacy_interface_setaddress($ifs, $addr)
{
    $cmd = '/sbin/ifconfig ' . escapeshellarg($ifs) . ' alias ' . escapeshellarg($addr);

    exec($cmd . ' 2>&1', $out, $ret);
    if ($ret) {
        log_error('The command `' . $cmd . '\' failed to execute');
    }
}

function legacy_interface_deladdress($ifs, $addr)
{
    $cmd = '/sbin/ifconfig ' . escapeshellarg($ifs) . ' -alias ' . escapeshellarg($addr);

    exec($cmd . ' 2>&1', $out, $ret);
    if ($ret) {
        log_error('The command `' . $cmd . '\' failed to execute');
    }
}

function legacy_interface_rename($ifs, $name)
{
    $cmd = '/sbin/ifconfig ' . escapeshellarg($ifs) . ' name ' . escapeshellarg($name);

    exec($cmd . ' 2>&1', $out, $ret);
    if ($ret) {
        log_error('The command `' . $cmd . '\' failed to execute');
    }
}

function legacy_interface_mtu($ifs, $mtu)
{
    $cmd = '/sbin/ifconfig ' . escapeshellarg($ifs) . ' mtu ' . escapeshellarg($mtu);

    exec($cmd . ' 2>&1', $out, $ret);
    if ($ret) {
        log_error('The command `' . $cmd . '\' failed to execute');
    }
}

function legacy_bridge_member($ifs, $member)
{
    $cmd = '/sbin/ifconfig ' . escapeshellarg($ifs) . ' addm ' . escapeshellarg($member);

    exec($cmd . ' 2>&1', $out, $ret);
    if ($ret) {
        log_error('The command `' . $cmd . '\' failed to execute');
    }
}

function legacy_vlan_tag($ifs, $member, $tag)
{
    $cmd = '/sbin/ifconfig ' . escapeshellarg($ifs) . ' vlandev ' . escapeshellarg($member) . ' vlan ' . escapeshellarg($tag);

    exec($cmd . ' 2>&1', $out, $ret);
    if ($ret) {
        log_error('The command `' . $cmd . '\' failed to execute');
    }
}

function legacy_interface_stats($ifs)
{
    $cmd = '/usr/local/sbin/ifinfo ' . escapeshellarg($ifs);
    $stats = array();

    exec($cmd . ' 2>&1', $out, $ret);
    if ($ret) {
        log_error('The command `' . $cmd . '\' failed to execute');
        return $stats;
    }

    if (count($out)) {
        /* first one is header */
        array_shift($out);

        foreach ($out as $line) {
            $stat = explode(':', $line);
            $stats[trim($stat[0])] = trim($stat[1]);
        }
    }

    return $stats;
}

/**
 * detect interface capabilities using ifconfig -m
 * @param $intf interface name
 * @return array list of interface capabilities (in lowercase)
 */
function legacy_interface_details($intf)
{
    $result = array();
    $result["capabilities"] = array();
    $result["options"] = array();
    $process = proc_open('/sbin/ifconfig -m ' . escapeshellarg($intf), array(array("pipe", "r"), array("pipe", "w")), $pipes);
    if (is_resource($process)) {
        $ifconfig_data = stream_get_contents($pipes[1]);
        foreach (explode("\n", $ifconfig_data) as $line) {
            if (strpos(trim($line), 'capabilities=') !== false) {
                // parse capabilities
                $capabilities = substr($line, strpos($line, '<') + 1, -1);
                foreach (explode(',', $capabilities) as $capability) {
                    $result["capabilities"][] = strtolower(trim($capability));
                }
            } elseif (strpos(trim($line), 'options=') !== false) {
                // parse options
                $capabilities = substr($line, strpos($line, '<') + 1, -1);
                foreach (explode(',', $capabilities) as $capability) {
                    $result["options"][] = strtolower(trim($capability));
                }
            }

        }
        fclose($pipes[1]);
        proc_close($process);
    }
    return $result;
}

function legacy_netgraph_attach($ifs)
{
	mwexecf('/usr/local/sbin/ngattach %s', array($ifs));
}

function legacy_netgraph_detach($ifs)
{
        mwexecf('/usr/sbin/ngctl msg %s: detach', array($ifs), true);
}

function legacy_netgraph_rename($tmpifs, $ifs)
{
        mwexecf('/usr/sbin/ngctl name %s: %s', array($tmpifs, $ifs));
}

/**
 * configure interface hardware settings
 * @param string $ifs interface name
 */
function configure_interface_hardware($ifs)
{
    global $config;
    $intf_details = legacy_interface_details($ifs);
    /* skip vlans for checksumming and polling */
    if (!stristr($ifs, "_vlan") && is_array($intf_details)) {
        // get current settings
        $csum_set = in_array('rxcsum', $intf_details['options']) || in_array('txcsum', $intf_details['options']);
        $tso_set = in_array('tso4', $intf_details['options']) || in_array('tso6', $intf_details['options']);
        $lro_set = in_array('lro', $intf_details['options']);
        $polling_set = in_array('polling', $intf_details['options']);

        // hardware checksum offloading offloading
        if (isset($config['system']['disablechecksumoffloading']) && $csum_set) {
            legacy_interface_flags($ifs, '-txcsum -rxcsum', false);
        } elseif (!isset($config['system']['disablechecksumoffloading']) && !$csum_set) {
            legacy_interface_flags($ifs, 'txcsum rxcsum', false);
        }
        // TCP segmentation offloading
        if (isset($config['system']['disablesegmentationoffloading']) && $tso_set) {
            legacy_interface_flags($ifs, '-tso', false);
        } elseif (!isset($config['system']['disablesegmentationoffloading']) && !$tso_set) {
            legacy_interface_flags($ifs, 'tso', false);
        }

        // large receive offload
        if (isset($config['system']['disablelargereceiveoffloading']) && $lro_set) {
            legacy_interface_flags($ifs, '-lro', false);
        } elseif (!isset($config['system']['disablelargereceiveoffloading']) && !$lro_set) {
            legacy_interface_flags($ifs, 'lro', false);
        }

        // polling
        if (isset($config['system']['polling']) && !$polling_set) {
            legacy_interface_flags($ifs, 'polling', false);
        } elseif (!isset($config['system']['polling']) && $polling_set) {
            legacy_interface_flags($ifs, '-polling', false);
        }
    }
}
