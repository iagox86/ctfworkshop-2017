<?php
  define('FLAG', 'FLAG:314499024ca35fa77155dec25b1509e6');

  $accts = array(
    'administrator' => 'bmLhrHjius',
    'guest' => 'guest',
  );

  function is_valid($username, $password)
  {
    global $accts;

    return array_key_exists($username, $accts) && $accts[$username] === $password;
  }
?>
