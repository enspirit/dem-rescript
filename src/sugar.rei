let or_else: (option('a), 'a) => 'a;
let ( || ) : (option('a), 'a) => 'a; // a || b <=> or_else(a,b)
let has_key: (Js.Dict.t('a), Js.Dict.key) => bool;
