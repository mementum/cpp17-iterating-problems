#!/usr/bin/env ruby
class PDFConverterCustomFootNotes < (Asciidoctor::Converter.for 'pdf')
  register_for 'pdf'

  def ink_footnotes node
    # used later as optiont to ink_prose
    align_left = "left"
    return if (fns = (doc = node.document).footnotes - @rendered_footnotes).empty?
    theme_margin :block, :bottom if node.context == :document || node == node.document.last_child
    theme_margin :footnotes, :top unless (valign_bottom = @theme.footnotes_margin_top == 'auto')
    with_dry_run do |extent|
      if valign_bottom && (single_page_height = extent&.single_page_height) && (delta = cursor - single_page_height - 0.0001) > 0
        move_down delta
      end
      theme_font :footnotes do
        (title = doc.attr 'footnotes-title') && (ink_caption title, category: :footnotes)
        item_spacing = @theme.footnotes_item_spacing
        index_offset = @rendered_footnotes.length
        sect_xreftext = node.context == :section && (node.xreftext node.document.attr 'xrefstyle')
        fns.each do |fn|
          label = (index = fn.index) - index_offset
          if sect_xreftext
            fn.singleton_class.send :attr_accessor, :label unless fn.respond_to? :label=
            fn.label = %(#{label} - #{sect_xreftext})
          end
          ink_prose %(<a id="_footnotedef_#{index}">#{DummyText}</a>[<a anchor="_footnoteref_#{index}">#{label}</a>] #{fn.text}), margin_bottom: item_spacing, hyphenate: false, align: align_left.to_sym
        end
        @rendered_footnotes += fns if extent
      end
    end
    nil
  end
end
