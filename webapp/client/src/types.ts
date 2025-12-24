export type ChapterInfo = {
  id: string;
  title: string;
};

export type ChapterContentHtml = ChapterInfo & {
  html: string;
};
