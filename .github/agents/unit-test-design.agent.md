---
name: "Unit Test Design"
description: "Unit test specification for a DFCS story. Use when given a Story ID (e.g. ST-010) whose design exists in docs/design, to produce a CSV test spec under docs/test-specs."
argument-hint: "Story ID, e.g. ST-010"
tools: [read, search, edit, execute]
handoffs:
  - label: Review Unit Test Spec
    agent: Unit Test Review
    prompt: Review the unit test spec CSV for the story above.
    send: false
---
Load the `unit-test-design` skill (`.github/skills/unit-test-design/SKILL.md`) and follow it for the given Story ID. It defines the sources, rules, procedure, CSV format, validator and final response.
