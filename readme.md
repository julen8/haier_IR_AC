[参考](https://bbs.hassbian.com/thread-20090-1-1.html)

使用方法：

remote_base 需要到 https://github.com/esphome/esphome/releases 下载对应的版本`esphome-2024.5.3/esphome/components/remote_base`

1. 将external_components/haier_ir 目录拷贝到 /homeassistant/esphome/external_components/
2. 拷贝external_components/remote_base/haier_protocol.h和haier_protocol.cpp 拷贝到 /config/esphome/external_components/remote_base/
3. 将external_components/remote_base/__init__.py 里面注释是 Haier 的部分复制过去

__init__.py:
```python

# Haier
HaierData, HaierBinarySensor, HaierTrigger, HaierAction, HaierDumper = declare_protocol(
    "Haier"
)
HaierAction = ns.class_("HaierAction", RemoteTransmitterActionBase)
HAIER_SCHEMA = cv.Schema(
    {
        cv.Required(CONF_CODE): cv.templatable(
            cv.All(
                [cv.Any(cv.hex_uint8_t, cv.uint8_t)],
                cv.Length(min=5, max=5),
            )
        ),
    }
)


@register_binary_sensor("haier", HaierBinarySensor, HAIER_SCHEMA)
def haier_binary_sensor(var, config):
    cg.add(var.set_code(config[CONF_CODE]))


@register_trigger("haier", HaierTrigger, HaierData)
def haier_trigger(var, config):
    pass


@register_dumper("haier", HaierDumper)
def haier_dumper(var, config):
    pass


@register_action(
    "haier",
    HaierAction,
    HAIER_SCHEMA,
)
async def haier_action(var, config, args):
    code_ = config[CONF_CODE]
    if cg.is_template(code_):
        template_ = await cg.templatable(code_, args, cg.std_vector.template(cg.uint8))
        cg.add(var.set_code_template(template_))
    else:
        cg.add(var.set_code_static(code_))
```

esphome:
```yaml
sensor:
  - platform: homeassistant
    id: ac_temperature
    entity_id: sensor.living_room_temperature

remote_receiver:
  id: rr1
  pin:
    number: 25
    inverted: True
  dump: raw

remote_transmitter:
  pin: 26
  carrier_duty_percent: 50%

external_components:
  - source:
      type: local
      path: external_components
    components: [ remote_base, haier_ir ]

climate:
  - platform: haier_ir
    name: "Living Room AC"
    sensor: ac_temperature
    receiver_id: rr1
```