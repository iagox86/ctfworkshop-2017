<?php
  define('SECRET', '25fab66r');
  define('FLAG', 'FLAG:cdc0357731d10aa24cbf634db1823749');

  $accts = array(
    'administrator' => 'bmLhVHjius',
    'guest' => 'guest',
  );

  function is_valid($username, $password)
  {
    global $accts;

    return array_key_exists($username, $accts) && $accts[$username] === $password;
  }
?>
