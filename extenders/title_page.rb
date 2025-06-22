class PDFConverterCustomTitlePage < (Asciidoctor::Converter.for 'pdf')
  register_for 'pdf'
  def ink_title_page doc
      # QUESTION: allow alignment per element on title page?
      title_text_align = (@theme.title_page_text_align || @base_text_align).to_sym

      theme_font :title_page do
          if (title_top = @theme.title_page_title_top)
              @y = resolve_top title_top
          end
          unless @theme.title_page_title_display == 'none'
              doctitle = doc.doctitle partition: true
              # change stringified \n in string to real newline
              thetitle = doc.attr 'thetitle'
              thetitle = thetitle.gsub(/\\n/, "\n")
              move_down @theme.title_page_title_margin_top || 0
              indent (@theme.title_page_title_margin_left || 0), (@theme.title_page_title_margin_right || 0) do
                  theme_font :title_page_title do
                      # ink "thetitle" add normalize to avoid endline normalization
                      ink_prose thetitle, align: title_text_align, margin: 0, normalize: false
                  end
              end
              move_down @theme.title_page_title_margin_bottom || 0
          end
          if @theme.title_page_subtitle_display != 'none' && (subtitle = (doctitle || (doc.doctitle partition: true)).subtitle)
              # change stringified \n in string to real newline
              subtitle = doc.attr 'thesubtitle'
              subtitle = subtitle.gsub(/\\n/, "\n")
              move_down @theme.title_page_subtitle_margin_top || 0
              indent (@theme.title_page_subtitle_margin_left || 0), (@theme.title_page_subtitle_margin_right || 0) do
                  theme_font :title_page_subtitle do
                      # add normalize to avoid endline normalization
                      ink_prose subtitle, align: title_text_align, margin: 0, normalize: false
                  end
              end
              move_down @theme.title_page_subtitle_margin_bottom || 0
          end
          if @theme.title_page_authors_display != 'none' && (doc.attr? 'authors')
              move_down @theme.title_page_authors_margin_top || 0
              indent (@theme.title_page_authors_margin_left || 0), (@theme.title_page_authors_margin_right || 0) do
                  generic_authors_content = @theme.title_page_authors_content
                  authors_content = {
                      name_only: @theme.title_page_authors_content_name_only || generic_authors_content,
                      with_email: @theme.title_page_authors_content_with_email || generic_authors_content,
                      with_url: @theme.title_page_authors_content_with_url || generic_authors_content,
                  }
                  authors = doc.authors.map.with_index do |author, idx|
                      with_author doc, author, idx == 0 do
                          author_content_key = (url = doc.attr 'url') ? ((url.start_with? 'mailto:') ? :with_email : :with_url) : :name_only
                          if (author_content = authors_content[author_content_key])
                              apply_subs_discretely doc, author_content, drop_lines_with_unresolved_attributes: true, imagesdir: @themesdir
                          else
                              doc.attr 'author'
                          end
                      end
                  end.join @theme.title_page_authors_delimiter
                  theme_font :title_page_authors do
                      ink_prose authors, align: title_text_align, margin: 0, normalize: true
                  end
              end
              move_down @theme.title_page_authors_margin_bottom || 0
          end
          unless @theme.title_page_revision_display == 'none' || (revision_info = [(doc.attr? 'revnumber') ? %(#{doc.attr 'version-label'} #{doc.attr 'revnumber'}) : nil, (doc.attr 'revdate')].compact).empty?
              move_down @theme.title_page_revision_margin_top || 0
              revision_text = revision_info.join @theme.title_page_revision_delimiter
              if (revremark = doc.attr 'revremark')
                  revision_text = %(#{revision_text}: #{revremark})
              end
              indent (@theme.title_page_revision_margin_left || 0), (@theme.title_page_revision_margin_right || 0) do
                  theme_font :title_page_revision do
                      ink_prose revision_text, align: title_text_align, margin: 0, normalize: false
                  end
              end
              move_down @theme.title_page_revision_margin_bottom || 0
          end
      end
  end
end
