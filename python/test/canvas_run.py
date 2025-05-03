import os
import argparse
from datetime import datetime
import inspect
import re
from string import Template
import textwrap

# Global header template stored as a cleandoc string for exact formatting
_GLOBAL_HEADER_TEMPLATE = inspect.cleandoc("""
/***********************************************************************************
*
*       ********************************************************************
*       ****    ${FILENAME_SPACED}  ____  F I L E    ****
*       ********************************************************************
*
*              AUTHOR:      ${AUTHOR}
*               DATED:      ${DATE}.
*
*       ********************************************************************
*                FILE:      [${FILE_PATH}]
*
*
*
**************************************************************************************
**************************************************************************************/
${CLASS_BODY}
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "${NAMESPACE}" NAMESPACE.




#endif      //  ${GUARD}  //
// *************************************************************************** //
// *************************************************************************** //
//
//  END.
""")

# Author metadata and date format
_AUTHOR = "Collin A. Bond"
_DATE_FORMAT = "%B %d, %Y"  # e.g., May 1, 2025

# Supported presets
_PRESETS = ["rule of 5", "rule of 7", "imgui"]

# Examples epilog with each invocation on its own indented line
_EXAMPLES = (
    "Examples:\n"
    "\tpython scaffold_generator.py MyClass -s imgui\n"
    "\tpython scaffold_generator.py NDMatrix --module include/nd -p "
    "--description \"N-D matrix\" -s \"rule of 5\""
)

# Custom help formatter to align options and indent sections with tabs
class TabHelpFormatter(argparse.HelpFormatter):
    def __init__(self, prog):
        super().__init__(prog, max_help_position=30)

    def format_help(self):
        help_text = super().format_help()
        lines = help_text.splitlines()
        new_lines = []
        for line in lines:
            stripped = line.lstrip()
            # Section headers
            if stripped.startswith('positional arguments:') or stripped.startswith('options:'):
                new_lines.append('\t' + stripped)
            # Epilog header
            elif stripped.startswith('Examples:'):
                new_lines.append('\t' + stripped)
            # Argument lines (start with '-' or alphanumeric under section)
            elif line.startswith('  ') and stripped:
                new_lines.append('\t' + stripped)
            else:
                new_lines.append(line)
        return '\n'.join(new_lines)

# CLI argument specifications
def _build_arg_spec():
    return {
        'class_name': {
            'args': ['class_name'],
            'kwargs': {'nargs': '?', 'default': 'ClassName', 'help': 'Name of the class'}
        },
        'module': {
            'args': ['--module'],
            'kwargs': {'default': '.', 'help': 'Directory for output files'}
        },
        'description': {
            'args': ['--description'],
            'kwargs': {'default': 'NO DESCRIPTION PROVIDED', 'help': 'Class description'}
        },
        'parents': {
            'args': ['-p', '--parents'],
            'kwargs': {'action': 'store_true', 'help': 'Create parent directories'}
        },
        'preset': {
            'args': ['-s', '--preset'],
            'kwargs': {
                'choices': _PRESETS,
                'metavar': 'PRESET',
                'default': 'rule of 5',
                'help': f'Class skeleton style (valid: {", ".join(_PRESETS)})'
            }
        }
    }


def spaced(text: str) -> str:
    """Insert a space between each character."""
    return " ".join(text)


def compute_guard(name: str) -> str:
    """
    Construct header guard macro:
    1) Normalize name to snake-case, lowercase.
    2) Uppercase.
    3) Prepend '_CBAPP_' and append '_H'.
    """
    base = re.sub(r'[^0-9a-zA-Z]', '_', name).lower()
    base = base.upper()
    return f"_CBAPP_{base}_H"


def write_file(path: str, content: str, parents: bool = False) -> None:
    if os.path.exists(path):
        raise FileExistsError(f"File '{path}' already exists")
    dirpath = os.path.dirname(path)
    if dirpath and not os.path.isdir(dirpath):
        if parents:
            os.makedirs(dirpath)
        else:
            raise FileNotFoundError(f"Directory '{dirpath}' does not exist")
    with open(path, 'w', encoding='utf-8') as f:
        f.write(content)


def create_class_skeleton(params: dict) -> str:
    cls  = params['class_name']
    desc = params['description']
    lines = []

    banner = '*' * 63

    # --- namespace open ---
    lines.append('namespace cb { // BEGINNING NAMESPACE "cb"...')
    lines.append(f'// {banner} //')
    lines.append(f'// {banner} //')
    lines.append('')

    # --- Primary Class Interface Banner ---
    lines.append(f'// {banner} //')
    lines.append(f'// {banner} //')
    lines.append('// 3.  PRIMARY CLASS INTERFACE')
    lines.append(f'// @brief {desc}')
    lines.append(f'// {banner} //')
    lines.append(f'// {banner} //')
    lines.append('')

    # class declaration
    lines.append(f'class {cls}')
    lines.append('{')
    lines.append('')

    # public section
    lines.append(f'// {banner} //')
    lines.append('public:')
    lines.append('    // 1.1  Default Constructor, Destructor, etc.    [src/…]')
    lines.append(f'    {cls}(void);')
    lines.append(f'    ~{cls}(void);')
    lines.append('')
    lines.append('    // 1.2  Primary Class Interface.                [src/…]')
    lines.append('    void Begin(bool* p_open = nullptr);')
    lines.append('')
    lines.append('    // 1.3  Deleted Operators, Functions, etc.')
    lines.append(f'    {cls}(const {cls}&)     = delete;')
    lines.append(f'    {cls}({cls}&&) noexcept = delete;')
    lines.append(f'    {cls}& operator=(const {cls}&)     = delete;')
    lines.append(f'    {cls}& operator=({cls}&&) noexcept = delete;')
    lines.append('')

    # protected data-members
    lines.append(f'// {banner} //')
    lines.append('protected:')
    lines.append('    // 2.A  PROTECTED DATA-MEMBERS…')
    lines.append('    bool m_running = true;')
    lines.append('    // … add more protected members here')
    lines.append('')

    # protected member functions
    lines.append('    // 2.B  PROTECTED MEMBER FUNCTIONS…')
    lines.append('    void init(void);')
    lines.append('    void load(void);')
    lines.append('    void destroy(void);')
    lines.append('')

    # private section
    lines.append(f'// {banner} //')
    lines.append('private:')
    lines.append('    // 3.  PRIVATE MEMBER FUNCTIONS…')
    lines.append('    // … add private helpers here')
    lines.append('')

    # close class
    lines.append(f'}}; // END "{cls}" INLINE CLASS DEFINITION.')
    lines.append('')

    return "\n".join(lines)


def generate_header_file(params: dict) -> None:
    cls = params['class_name']
    module = params['module']
    date_str = datetime.now().strftime(_DATE_FORMAT)
    tpl = Template(_GLOBAL_HEADER_TEMPLATE)
    filled = tpl.substitute({
        'FILENAME_SPACED': spaced(f"{cls}.h"),
        'AUTHOR': _AUTHOR,
        'DATE': date_str,
        'FILE_PATH': os.path.join(module, f"{cls}.h"),
        'CLASS_BODY': create_class_skeleton(params),
        'NAMESPACE': 'cb',
        'GUARD': compute_guard(cls)
    })
    h_path = os.path.join(module, f"{cls}.h")
    write_file(h_path, filled, parents=params['parents'])
    print(f"Generated header: {h_path}")


def generate_cpp_file(params: dict) -> None:
    cls = params['class_name']
    module = params['module']
    preset = params['preset']
    cpp_path = os.path.join(module, f"{cls}.cpp")
    lines = []
    lines.append(f'#include "{cls}.h"\n')
    if preset == 'imgui':
        lines.append('//=== Constructors & Destructor ===')
        lines.append(f"{cls}::{cls}() {{}}")
        lines.append(f"{cls}::~{cls}() {{}}\n")
        lines.append('//=== Public Methods ===')
        lines.append(f"void {cls}::Begin(bool* p_open) {{")
        lines.append('    // TODO: implement Begin logic')
        lines.append('}')
    else:
        lines.append(f"// TODO: implement {preset} .cpp for {cls}")
    content = "\n".join(lines)
    write_file(cpp_path, content, parents=params['parents'])
    print(f"Generated implementation: {cpp_path}")


def main():
    parser = argparse.ArgumentParser(
        description='Generate C++ class scaffold',
        add_help=False,
        formatter_class=TabHelpFormatter,
        epilog=_EXAMPLES
    )
    # Custom help flag
    parser.add_argument(
        '-h', '-H', '--help',
        action='help',
        help='Show this help message and exit'
    )
    # Register other arguments
    for spec in _build_arg_spec().values():
        parser.add_argument(*spec['args'], **spec['kwargs'])
    args = parser.parse_args()
    params = vars(args)

    generate_header_file(params)
    generate_cpp_file(params)


if __name__ == '__main__':
    main()
