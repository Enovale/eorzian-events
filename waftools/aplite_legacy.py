from waflib.Configure import conf


@conf
def appinfo_bitmap_to_png(ctx, appinfo_json):
    if not ctx.supports_bitmap_resource():
        for res in appinfo_json['pebble']['resources']['media']:
            if res['type'] == 'bitmap':
                res['type'] = 'png'
