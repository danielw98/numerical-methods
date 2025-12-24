import type { ChapterContentHtml, ChapterInfo } from './types';

export async function listChapters(prefix?: string): Promise<ChapterInfo[]> {
  const url = prefix ? `/api/chapters?prefix=${encodeURIComponent(prefix)}` : '/api/chapters';
  const res = await fetch(url);
  if (!res.ok) {
    throw new Error(`Failed to list chapters: ${res.status}`);
  }
  return res.json();
}

export async function getChapterHtml(id: string, prefix?: string): Promise<ChapterContentHtml> {
  const url = prefix
    ? `/api/chapters/${encodeURIComponent(id)}?prefix=${encodeURIComponent(prefix)}`
    : `/api/chapters/${encodeURIComponent(id)}`;
  const res = await fetch(url);
  if (!res.ok) {
    throw new Error(`Failed to fetch chapter: ${res.status}`);
  }
  return res.json();
}
