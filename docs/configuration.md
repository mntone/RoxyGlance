# Configuration

Roxy Glance reads user settings from the YAML file `roxyg_user.yaml` in the app package's `LocalState` folder.

```text
%LOCALAPPDATA%\Packages\<PackageFamilyName>\LocalState\roxyg_user.yaml
```

## Applying Changes

Currently, settings are loaded when Roxy Glance starts. Restart the app after editing `roxyg_user.yaml` to apply the changes.

## Basic Structure

Settings are defined as a list of rules. Each rule can specify a name, one or more triggers, window filters, and one or more actions.

```yaml
rules:
  - name: "Example rule"
    when: [show]
    where:
      process: '^C:\Program Files\Example\example.exe$'
    then:
      - type: absolute_move_and_resize
        x: 0
        y: 0
        width: 1280
        height: 720
```

## Rule Fields

Each item in `rules` is a rule. Its fields are:

| Field | Description | If omitted |
| --- | --- | --- |
| `name` | Rule name | No name is set |
| `when` | Trigger that causes the rule to run; specify one value or a list | No triggers are set, so the rule does not run |
| `where` | Match Conditions that determine which windows the rule applies to | No conditions are specified |
| `then` | Action to perform; specify one action or a list of actions | No actions are performed |

The order of fields within a rule does not affect how it is read. See [Triggers](#triggers), [Filters](#filters), and [Actions](#actions) for the supported values and details.

## Triggers

The `when` field selects the event that triggers a rule:

- `init`: Application Started
- `focus`: Window Focused
- `show`: Window Shown

Trigger values are case-insensitive.

## Filters

In the GUI, filters are called **Match Conditions**. In YAML, define them under `where`:

| Key | Matches |
| --- | --- |
| `process` | Full process image path, including its directory and executable name |
| `class` | Window class |
| `title` | Window title |

String values use optional markers to select the comparison:

String comparisons are case-sensitive.

| Pattern | Comparison | Example |
| --- | --- | --- |
| `text` | Contains | `Editor` |
| `^text` | Starts with | `^Editor` |
| `text$` | Ends with | `.exe$` |
| `^text$` | Equals | `^Editor.exe$` |

For example:

```yaml
where:
  process: '^C:\Program Files\Editor\editor.exe$'
  class: ^Editor
  title: Untitled
```

## Actions

Define a single action or a list of actions under `then`. Action type names are case-insensitive. The examples use a list.

### Absolute Move and Resize

Use `absolute_move_and_resize` to set the window position and size in pixels. The integer values for `x`, `y`, `width`, and `height` must each be in the range `0` to `0x4000` (16,384), inclusive.

```yaml
then:
  - type: absolute_move_and_resize
    x: 0
    y: 0
    width: 1280
    height: 720
```

### Relative Move and Resize

Use `relative_move_and_resize` to set the window position and size relative to a monitor's work area. The floating-point values for `x`, `y`, `width`, and `height` must each be in the range `0.0` to `1.0`, inclusive. The optional `id` selects a monitor; explicit IDs must be integers from `1` to `16`. If omitted, the primary monitor is used.

`width` and `height` are fractions of the work area's size. `x` and `y` position the window within the remaining space after accounting for its size: `0.0` aligns it to the left or top edge, and `1.0` aligns it to the right or bottom edge.

```yaml
then:
  - type: relative_move_and_resize
    id: 1
    x: 0.0
    y: 0.0
    width: 1.0
    height: 1.0
```
