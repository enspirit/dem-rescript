let extension: string => option(string);
let read_text: string => option(string);
let read_json_data: string => option(Js.Json.t);
let read_yaml_data: string => option(Js.Json.t);
let read_js_data: string => option(Js.Json.t);
let read_data: option(string) => option(Js.Json.t);
let read_compilation_template: string => option(string);
let read_compilation_style: string => option(string);
