let extension: string => option<string>
let read_text: string => option<string>
let read_json_data: (~batch: bool=?, string) => list<Js.Json.t>
let read_yaml_data: (~batch: bool=?, string) => list<Js.Json.t>
let read_js_data: (~batch: bool=?, ~async: bool=?, string) => Js.Promise.t<list<Js.Json.t>>
let read_data: (~batch: bool=?, ~async: bool=?, option<string>) => Js.Promise.t<list<Js.Json.t>>
let read_single_data: option<string> => Js.Promise.t<option<Js.Json.t>>
let read_template: string => option<string>
let read_sass_style: string => option<string>
let read_style: option<string> => option<string>
let find: (~root: string=?, string) => option<string>
let build_partials: (~root: string=?, ~partials: Js.Dict.t<string>=?, list<string>) => Js.Dict.t<string>
let make_absolute_filepath: (~ext_before: string=?, ~ext_after: string=?, string) => string
let expand: (option<Js.Json.t>, option<string>) => option<string>
let ensure_path: string => unit
let write_md: (~output_filename_opt: option<string>, ~text_filename: string=?, string) => string
let write_html: (~output_filename_opt: option<string>, ~text_filename: string=?, string) => string
