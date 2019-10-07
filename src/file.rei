let extension: string => option(string);
let read_text: string => option(string);
let read_json_data: string => option(Js.Json.t);
let read_yaml_data: string => option(Js.Json.t);
let read_js_data: string => option(Js.Json.t);
let read_data: option(string) => option(Js.Json.t);
let read_template: string => option(string);
let read_style: string => option(string);
let find: (~root:string=?, string) => option(string);
let build_partials: (~root:string=?, ~partials:Js.Dict.t(string)=?, list(string)) => Js.Dict.t(string);
let write_html: (~output_filename:string=?, string, string) => string;
